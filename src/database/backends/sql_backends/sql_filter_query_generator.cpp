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

#include "sql_filter_query_generator.hpp"

#include <QStringList>
#include <QRegExp>

#include "tables.hpp"


namespace Database
{

    struct FilterData
    {
        enum Join
        {
            TagsWithPhotos,
            FlagsWithPhotos,
            Sha256WithPhotos,
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


    struct Generator final
    {
        Generator();
        ~Generator();

        QString parse(const std::vector<IFilter::Ptr> &);
        bool canBeMerged(const FilterData &, const FilterData &) const;
        QString nest(const QString& current, const FilterData& incoming);
        QString constructQuery(const FilterData &) const;
        QString getPhotoId() const;

        int level = 0;
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
        QString getFlagName(Photo::FlagsE flag) const
        {
            QString result;

            switch(flag)
            {
                case Photo::FlagsE::StagingArea:     result = FLAG_STAGING_AREA;  break;
                case Photo::FlagsE::ExifLoaded:      result = FLAG_TAGS_LOADED;   break;
                case Photo::FlagsE::Sha256Loaded:    result = FLAG_SHA256_LOADED; break;
                case Photo::FlagsE::ThumbnailLoaded: result = FLAG_THUMB_LOADED;  break;
                case Photo::FlagsE::GeometryLoaded:  result = FLAG_GEOM_LOADED;   break;
            }

            return result;
        }

        // IFilterVisitor interface
        void visit(EmptyFilter *) override
        {
        }

        void visit(FilterPhotosWithTag* desciption) override
        {
            m_filterResult.joins.insert(FilterData::TagsWithPhotos);

            if (desciption->tagValue.type() != TagValue::Type::Empty)
                m_filterResult.conditions.append(QString(TAB_TAGS ".name = '%1' AND " TAB_TAGS ".value = '%2'")
                                                 .arg(desciption->tagName.getTag() )
                                                 .arg(desciption->tagValue.rawValue())
                                                );
            else
                m_filterResult.conditions.append(QString(TAB_TAGS ".name = '%1'").arg(desciption->tagName.getTag()));
        }

        void visit(FilterPhotosWithFlags* flags) override
        {
            m_filterResult.joins.insert(FilterData::FlagsWithPhotos);

            QStringList conditions;

            for(const auto& it: flags->flags)
            {
                const QString flagName = getFlagName(it.first);
                const int flagValue = it.second;

                conditions.append(QString(TAB_FLAGS ".%1 = '%2'")
                                  .arg(flagName)
                                  .arg(flagValue));
            }

            QString merged_conditions;

            switch (flags->mode)
            {
                case FilterPhotosWithFlags::Mode::And:
                    merged_conditions = conditions.join(" AND ");
                    break;

                case FilterPhotosWithFlags::Mode::Or:
                    merged_conditions = "( " + conditions.join(" OR ") + " )";
                    break;
            }

            m_filterResult.conditions.append(merged_conditions);
        }

        void visit(FilterPhotosWithSha256* sha256) override
        {
            assert(sha256->sha256.isEmpty() == false);

            m_filterResult.joins.insert(FilterData::Sha256WithPhotos);
            m_filterResult.conditions.append( QString(TAB_SHA256SUMS ".sha256 = '%1'").arg(sha256->sha256.constData()) );
        }

        void visit(FilterNotMatchingFilter* filter) override
        {
            Generator generator;
            const QString internal_condition = generator.parse( {filter->filter} );

            //http://stackoverflow.com/questions/367863/sql-find-records-from-one-table-which-dont-exist-in-another
            m_filterResult.conditions.append( QString("photos.id NOT IN (%1)").arg(internal_condition) );
        }

        void visit(FilterPhotosWithId* filter) override
        {
            // No joins required
            m_filterResult.conditions.append( QString(TAB_PHOTOS ".id = '%1'").arg(filter->filter) );
        }

        void visit(FilterPhotosMatchingExpression* filter) override
        {
            const SearchExpressionEvaluator::Expression conditions = filter->expression;
            QString final_condition;

            final_condition += "(";
            const std::size_t s = conditions.size();

            for(std::size_t i = 0; i < s; i++)
            {
                const QString condition = conditions[i].m_value;

                if (conditions[i].m_exact)
                    final_condition += QString(TAB_TAGS ".value = '%1'").arg(condition);
                else
                    final_condition += QString(TAB_TAGS ".value LIKE '%%1%'").arg(condition);

                if (i + 1 < s)
                    final_condition += " OR ";
            }

            final_condition += ")";

            m_filterResult.joins.insert(FilterData::TagsWithPhotos);
            m_filterResult.conditions.append(final_condition);
        }

        void visit(FilterPhotosWithPath* filter) override
        {
            m_filterResult.conditions.append(QString(TAB_PHOTOS ".path = '%1'")
                                             .arg(filter->path));
        }

        void visit(FilterPhotosWithRole* filter) override
        {
            switch(filter->m_role)
            {
                case FilterPhotosWithRole::Role::Regular:
                    m_filterResult.conditions.append("photos.id NOT IN "
                                                        "("
                                                        "SELECT groups_members.photo_id FROM groups_members "
                                                        "UNION "
                                                        "SELECT groups.representative_id FROM groups"
                                                        ")"
                    );
                break;

                case FilterPhotosWithRole::Role::GroupRepresentative:
                    m_filterResult.conditions.append("photos.id IN "
                                                        "("
                                                        "SELECT groups.representative_id FROM groups"
                                                        ")"
                    );
                break;

                case FilterPhotosWithRole::Role::GroupMember:
                    m_filterResult.conditions.append("photos.id IN "
                                                        "("
                                                        "SELECT groups_members.photo_id FROM groups_members"
                                                        ")"
                    );
                break;
            }
        }

        FilterData m_filterResult;
    };


    ////////////////////////////////////////////////////////////////////////////


    Generator::Generator()
    {

    }


    Generator::~Generator()
    {

    }


    QString Generator::parse(const std::vector<IFilter::Ptr>& filters)
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


    bool Generator::canBeMerged(const FilterData& fd1, const FilterData& fd2) const
    {
        auto tf1 = fd1.joins.find(FilterData::TagsWithPhotos);
        auto tf2 = fd2.joins.find(FilterData::TagsWithPhotos);

        // there cannot be two joins for tags
        const bool result = tf1 == fd1.joins.cend() || tf2 == fd2.joins.cend();

        return result;
    }


    QString Generator::nest(const QString& current, const FilterData& incoming)
    {
        QString result;

        if (current.isEmpty())
        {
            const QString partial = constructQuery(incoming);
            result = "SELECT photos.id AS photos_id FROM " TAB_PHOTOS;
            result += partial;
        }
        else
        {
            result = "SELECT photos_id FROM ( " + current + ") AS level_%1_query";
            result = result.arg(++level);

            const QString partial = constructQuery(incoming);
            result += partial;
        }

        return result;
    }


    QString Generator::constructQuery(const FilterData& filterData) const
    {
        QString result;

        //fill JOIN section
        if (filterData.joins.empty() == false)  //at least one join
            result += " JOIN (";

        QStringList joinsWith;
        for(const auto& item: filterData.joins)
        {
            QString joinWith;

            switch(item)
            {
                case FilterData::TagsWithPhotos:   joinWith = TAB_TAGS;       break;
                case FilterData::FlagsWithPhotos:  joinWith = TAB_FLAGS;      break;
                case FilterData::Sha256WithPhotos: joinWith = TAB_SHA256SUMS; break;
            }

            joinsWith.append(joinWith);
        }

        result += joinsWith.join(", ");

        if (filterData.joins.empty() == false)  //at least one join
            result += ") ON (";

        QStringList joins;
        for(const auto& item: filterData.joins)
        {
            QString join;

            switch(item)
            {
                case FilterData::TagsWithPhotos:   join = TAB_TAGS ".photo_id = " + getPhotoId();   break;
                case FilterData::FlagsWithPhotos:  join = TAB_FLAGS ".photo_id = " + getPhotoId();  break;
                case FilterData::Sha256WithPhotos: join = TAB_SHA256SUMS ".photo_id = " + getPhotoId(); break;
            }

            joins.append(join);
        }

        result += joins.join(" AND ");

        if (filterData.joins.empty() == false)  //at least one join
            result += ")";

        //conditions
        if (filterData.conditions.isEmpty() == false)
        {
            result += " WHERE ";
            result += filterData.conditions.join(" AND ");
        }

        return result;
    }


    QString Generator::getPhotoId() const
    {
        return level == 0? "photos.id" : "photos_id";   //Use directly photos.id for non-nested queries. For nested one - use photos.id alias.
    }


    SqlFilterQueryGenerator::SqlFilterQueryGenerator()
    {

    }


    SqlFilterQueryGenerator::~SqlFilterQueryGenerator()
    {

    }


    QString SqlFilterQueryGenerator::generate(const std::vector<IFilter::Ptr>& filters) const
    {
        return Generator().parse(filters);
    }


    QString Database::SqlFilterQueryGenerator::generate(const QString& query) const
    {
        QRegExp exp("([^ ]+) ([^ ]+) ?([^ ]+)? ?(.*)?");

        const bool matched = exp.exactMatch(query);
        assert(matched);

        const QStringList captured = exp.capturedTexts();
        const QString item = captured[2];

        if (item == "photos")
            return forPhotos(captured);
        else
            assert(!"unknown item to fetch");

        return QString();
    }


    QString Database::SqlFilterQueryGenerator::forPhotos(const QStringList& expression) const
    {
        const QString scope =     expression[1];
        const QString operand =   expression[3];
        const QString condition = expression[4];

        QString result = "SELECT photos.id AS photos_id FROM " TAB_PHOTOS;

        return result;
    }

}
