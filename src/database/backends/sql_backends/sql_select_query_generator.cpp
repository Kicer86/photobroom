/*
 * Generator of SELECT queries.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "sql_select_query_generator.hpp"

#include <QStringList>

#include "tables.hpp"


namespace Database
{
    struct FilterData
    {
        enum Join
        {
            TagsWithPhotos,
            TagNamesWithTags,
            FlagsWithPhotos,
            HashWithPhotos,
        };

        std::set<Join> joins;
        QStringList conditions;

        FilterData(): joins(), conditions() {}

        void clear()
        {
            joins.clear();
            conditions.clear();
        }
    };


    struct SqlFiltersVisitor: Database::IFilterVisitor
    {
        SqlFiltersVisitor(): m_filterResult() {}
        virtual ~SqlFiltersVisitor() {}

        FilterData visit(const IFilter::Ptr& filter)
        {
            m_filterResult.clear();
            filter->visitMe(this);

            return m_filterResult;
        }

    private:
        QString getFlagName(IPhotoInfo::FlagsE flag) const
        {
            QString result;

            switch(flag)
            {
                case IPhotoInfo::FlagsE::StagingArea:     result = FLAG_STAGING_AREA; break;
                case IPhotoInfo::FlagsE::ExifLoaded:      result = FLAG_TAGS_LOADED;  break;
                case IPhotoInfo::FlagsE::Sha256Loaded:    result = FLAG_HASH_LOADED;  break;
                case IPhotoInfo::FlagsE::ThumbnailLoaded: result = FLAG_THUMB_LOADED; break;
            }

            return result;
        }

        // IFilterVisitor interface
        void visit(EmptyFilter *) override
        {
        }

        void visit(FilterPhotosWithTag* desciption) override
        {
            m_filterResult.joins.insert(FilterData::TagNamesWithTags);
            m_filterResult.joins.insert(FilterData::TagsWithPhotos);
            m_filterResult.conditions.append(QString(TAB_TAG_NAMES ".name = '%1' AND " TAB_TAGS ".value = '%2'")
                                             .arg(desciption->tagName)
                                             .arg(desciption->tagValue));
        }

        void visit(FilterPhotosWithFlags* flags) override
        {
            const QString flagName = getFlagName(flags->flag);

            m_filterResult.joins.insert(FilterData::FlagsWithPhotos);
            m_filterResult.conditions.append(QString(TAB_FLAGS ".%1 = '%2'")
            .arg(flagName)
            .arg(flags->value));
        }

        void visit(FilterPhotosWithSha256* sha256) override
        {
            assert(sha256->sha256.empty() == false);

            m_filterResult.joins.insert(FilterData::HashWithPhotos);
            m_filterResult.conditions.append( QString(TAB_HASHES ".hash = '%1'").arg(sha256->sha256.c_str()) );
        }

        void visit(FilterPhotosWithoutTag* filter) override
        {
            //http://stackoverflow.com/questions/367863/sql-find-records-from-one-table-which-dont-exist-in-another
            m_filterResult.conditions.append( QString("photos.id NOT IN (SELECT " TAB_TAGS ".photo_id FROM " TAB_TAGS
                                                      " JOIN " TAB_TAG_NAMES " ON ( " TAB_TAG_NAMES ".id = " TAB_TAGS ".name_id) "
                                                      " WHERE " TAB_TAG_NAMES ".name = '%1')")
                                              .arg(filter->tagName) );
        }

        void visit(FilterOrOperator* filter) override
        {
            FilterData orFilterData;
            QString condition;

            condition = "( ";

            const size_t s = filter->filters.size();

            QStringList conditions;
            for(size_t i = 0; i < s; i++)
            {
                m_filterResult.clear();
                filter->filters[i]->visitMe(this);

                orFilterData.joins.insert(m_filterResult.joins.cbegin(), m_filterResult.joins.cend());
                conditions.append(m_filterResult.conditions);
            }

            condition += conditions.join(" OR ");
            condition += " )";

            orFilterData.conditions.append(condition);
            m_filterResult = orFilterData;
        }

        FilterData m_filterResult;
    };


    struct Generator final
    {
        Generator() {}
        ~Generator() {}

        QString parse(const std::deque<IFilter::Ptr>& filters)
        {
            const size_t s = filters.size();
            SqlFiltersVisitor visitor;
            FilterData filterData;

            for (size_t i = 0; i < s; i++)
            {
                const size_t index = s - i - 1;
                FilterData currentfilterData = visitor.visit(filters[index]);

                filterData.joins.insert(currentfilterData.joins.cbegin(), currentfilterData.joins.cend());
                filterData.conditions.append(currentfilterData.conditions);
            }

            const QString result = constructQuery(filterData);

            return result;
        }

        QString constructQuery(const FilterData& filterData) const
        {
            QString result;

            result = "SELECT photos.id AS photos_id FROM " TAB_PHOTOS;

            //fill JOIN section
            if (filterData.joins.empty() == false)  //at least one join
                result += " JOIN (";

            for(auto it = filterData.joins.cbegin(); it != filterData.joins.cend();)
            {
                const auto join = *it;

                switch(join)
                {
                    case FilterData::TagsWithPhotos:   result += TAB_TAGS;      break;
                    case FilterData::TagNamesWithTags: result += TAB_TAG_NAMES; break;      //TAB_TAGS must be already joined
                    case FilterData::FlagsWithPhotos:  result += TAB_FLAGS;     break;
                    case FilterData::HashWithPhotos:   result += TAB_HASHES;    break;
                }

                ++it;
                if ( it != filterData.joins.cend())
                    result += ", ";
            }

            if (filterData.joins.empty() == false)  //at least one join
                result += ") ON (";

            for(auto it = filterData.joins.cbegin(); it != filterData.joins.cend();)
            {
                const auto join = *it;

                switch(join)
                {
                    case FilterData::TagsWithPhotos:   result += TAB_TAGS ".photo_id = " TAB_PHOTOS ".id";   break;
                    case FilterData::TagNamesWithTags: result += TAB_TAGS ".name_id = " TAB_TAG_NAMES ".id"; break;
                    case FilterData::FlagsWithPhotos:  result += TAB_FLAGS ".photo_id = " TAB_PHOTOS ".id";  break;
                    case FilterData::HashWithPhotos:   result += TAB_HASHES ".photo_id = " TAB_PHOTOS ".id"; break;
                }

                ++it;
                if ( it != filterData.joins.cend())
                    result += " AND ";
            }

            if (filterData.joins.empty() == false)  //at least one join
                result += ")";

            //conditions
            if (filterData.conditions.isEmpty() == false)
            {
                result += " WHERE ";

                for (auto it = filterData.conditions.cbegin(); it != filterData.conditions.cend();)
                {
                    const QString condition = *it;
                    result += condition;

                    ++it;

                    if (it != filterData.conditions.cend())
                        result += " AND ";
                }
            }

            return result;
        }
    };



    SqlSelectQueryGenerator::SqlSelectQueryGenerator()
    {

    }


    SqlSelectQueryGenerator::~SqlSelectQueryGenerator()
    {

    }


    QString SqlSelectQueryGenerator::generate(const std::deque<IFilter::Ptr>& filters)
    {
        return Generator().parse(filters);
    }

}
