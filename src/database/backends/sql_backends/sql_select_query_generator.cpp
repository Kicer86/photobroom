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

        bool empty() const
        {
            return joins.empty() && conditions.empty();
        }
    };


    struct FiltersVisitor final: Database::IFilterVisitor
    {
        FiltersVisitor(): m_filterResult() {}
        virtual ~FiltersVisitor() {}

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
            m_filterResult.conditions.append( QString("photos_id NOT IN (SELECT " TAB_TAGS ".photo_id FROM " TAB_TAGS
                                                      " JOIN " TAB_TAG_NAMES " ON ( " TAB_TAG_NAMES ".id = " TAB_TAGS ".name_id)"
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
            FiltersVisitor visitor;
            FilterData filterData;
            QString result;

            for (const IFilter::Ptr& filter: filters)
            {
                FilterData currentfilterData = visitor.visit(filter);

                const bool mergable = canBeMerged(currentfilterData, filterData);

                if (mergable)
                {
                    filterData.joins.insert(currentfilterData.joins.cbegin(), currentfilterData.joins.cend());
                    filterData.conditions.append(currentfilterData.conditions);
                }
                else   //flush filter to QString query
                {
                    //flush current data
                    result = nest(result, filterData);

                    //apply new one
                    filterData = currentfilterData;
                }
            }

            //final flush
            if (filterData.empty() == false || result.isEmpty())   //flush when there is somethign to be flushed or, we have empty queue (no filters case)
            {
                result = nest(result, filterData);
                filterData.clear();
            }

            return result;
        }


        bool canBeMerged(const FilterData& fd1, const FilterData& fd2) const
        {
            auto tf1 = fd1.joins.find(FilterData::TagsWithPhotos);
            auto tf2 = fd2.joins.find(FilterData::TagsWithPhotos);

            // there cannot be two joins for tags
            const bool result = tf1 == fd1.joins.cend() || tf2 == fd2.joins.cend();

            return result;
        }


        QString nest(const QString& current, const QString& incoming) const
        {
            QString result;

            if (current.isEmpty())
            {
                result = "SELECT photos.id AS photos_id FROM " TAB_PHOTOS;
                result += incoming;
            }
            else
                result = "SELECT photos_id FROM ( " + current + ")" + incoming;

            return result;
        }


        QString nest(const QString& current, const FilterData& incoming) const
        {
            const QString partial = constructQuery(incoming);
            const QString result = nest(current, partial);

            return result;
        }


        QString constructQuery(const FilterData& filterData) const
        {
            QString result;

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
                    case FilterData::TagsWithPhotos:   result += TAB_TAGS ".photo_id = photos_id";   break;
                    case FilterData::TagNamesWithTags: result += TAB_TAGS ".name_id = " TAB_TAG_NAMES ".id"; break;
                    case FilterData::FlagsWithPhotos:  result += TAB_FLAGS ".photo_id = photos_id";  break;
                    case FilterData::HashWithPhotos:   result += TAB_HASHES ".photo_id = photos_id"; break;
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
