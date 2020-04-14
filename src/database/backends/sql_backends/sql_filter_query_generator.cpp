/*
 * Photo Broom - photos management tool.
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

#include "tables.hpp"


namespace Database
{

    struct Generator final
    {
        Generator();
        ~Generator();

        QString parse(const std::vector<IFilter::Ptr> &);

        int level = 0;
    };


    struct FiltersVisitor final: Database::IFilterVisitor
    {
        FiltersVisitor() = default;
        ~FiltersVisitor() = default;

        QString visit(const IFilter::Ptr& filter)
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
            QString condition;

            QString comparisonType = "=";

            switch (desciption->valueMode)
            {
                case FilterPhotosWithTag::ValueMode::Greater:        comparisonType = ">";  break;
                case FilterPhotosWithTag::ValueMode::GreaterOrEqual: comparisonType = ">="; break;
                case FilterPhotosWithTag::ValueMode::LessOrEqual:    comparisonType = "<="; break;
                case FilterPhotosWithTag::ValueMode::Less:           comparisonType = "<";  break;
                default: break;
            }

            if (desciption->tagValue.type() != Tag::ValueType::Empty)
                condition = QString(TAB_TAGS ".name = '%1' AND " TAB_TAGS ".value %3 '%2'")
                                        .arg(desciption->tagType )
                                        .arg(desciption->tagValue.rawValue())
                                        .arg(comparisonType);
            else
                condition = QString(TAB_TAGS ".name = '%1'").arg(desciption->tagType);

            m_filterResult = QString("SELECT %1.id FROM %1 JOIN (%2) ON (%2.photo_id = %1.id) WHERE %3")
                                .arg(TAB_PHOTOS)
                                .arg(TAB_TAGS)
                                .arg(condition);
        }

        void visit(FilterPhotosWithFlags* flags) override
        {
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

            m_filterResult = QString("SELECT %1.id FROM %1 JOIN (%2) ON (%2.photo_id = %1.id) WHERE %3")
                                .arg(TAB_PHOTOS)
                                .arg(TAB_FLAGS)
                                .arg(merged_conditions);
        }

        void visit(FilterPhotosWithSha256* sha256) override
        {
            assert(sha256->sha256.isEmpty() == false);

            m_filterResult = QString("SELECT id FROM %1 JOIN (%2) ON (%2.photo_id = %1.id) WHERE %2.sha256 = '%3'")
                                .arg(TAB_PHOTOS)
                                .arg(TAB_SHA256SUMS)
                                .arg(sha256->sha256.constData());
        }

        void visit(FilterNotMatchingFilter* filter) override
        {
            Generator generator;
            const QString internal_condition = generator.parse( {filter->filter} );

            //http://stackoverflow.com/questions/367863/sql-find-records-from-one-table-which-dont-exist-in-another
            m_filterResult = QString("SELECT id FROM %1 WHERE id NOT IN (%2)")
                                .arg(TAB_PHOTOS)
                                .arg(internal_condition);
        }

        void visit(FilterPhotosWithId* filter) override
        {
            m_filterResult = QString("SELECT id FROM %1 WHERE id = '%2'")
                                .arg(TAB_PHOTOS)
                                .arg(filter->filter);
        }

        void visit(FilterPhotosMatchingExpression* filter) override
        {
            const SearchExpressionEvaluator::Expression conditions = filter->expression;
            const std::size_t s = conditions.size();

            QString tags_conditions;
            QString people_conditions;

            for(std::size_t i = 0; i < s; i++)
            {
                const QString condition = conditions[i].m_value;

                if (conditions[i].m_exact)
                {
                    tags_conditions += QString("%1.value = '%2'")
                                           .arg(TAB_TAGS)
                                           .arg(condition);

                    people_conditions += QString("%1.name = '%2'")
                                             .arg(TAB_PEOPLE_NAMES)
                                             .arg(condition);
                }
                else
                {
                    tags_conditions += QString("%1.value LIKE '%%2%'")
                                           .arg(TAB_TAGS)
                                           .arg(condition);

                    people_conditions += QString("%1.name LIKE '%%2%'")
                                             .arg(TAB_PEOPLE_NAMES)
                                             .arg(condition);
                }

                if (i + 1 < s)
                {
                    tags_conditions += " OR ";
                    people_conditions += " OR ";
                }
            }

            const QString tags_query = QString("SELECT %1.id FROM %1 JOIN (%2) ON (%1.id = %2.photo_id) WHERE (%3)")
                                           .arg(TAB_PHOTOS)
                                           .arg(TAB_TAGS)
                                           .arg(tags_conditions);

            const QString people_query = QString("SELECT %1.id FROM %1 JOIN (%2, %3) ON (%1.id = %2.photo_id AND %2.person_id = %3.id) WHERE (%4)")
                                             .arg(TAB_PHOTOS)
                                             .arg(TAB_PEOPLE)
                                             .arg(TAB_PEOPLE_NAMES)
                                             .arg(people_conditions);

            m_filterResult = QString("SELECT %1.id FROM %1 WHERE %1.id IN (%2) OR %1.id IN (%3)")
                                 .arg(TAB_PHOTOS)
                                 .arg(tags_query)
                                 .arg(people_query);
        }

        void visit(FilterPhotosWithPath* filter) override
        {
            m_filterResult = QString("SELECT %1.id FROM %1 WHERE %1.path = '%2'")
                                .arg(TAB_PHOTOS)
                                .arg(filter->path);
        }

        void visit(FilterPhotosWithRole* filter) override
        {
            switch(filter->m_role)
            {
                case FilterPhotosWithRole::Role::Regular:
                    m_filterResult = QString("SELECT id FROM %1 WHERE id NOT IN "
                                                    "("
                                                        "SELECT groups_members.photo_id FROM groups_members "
                                                        "UNION "
                                                        "SELECT groups.representative_id FROM groups"
                                                    ")")
                                        .arg(TAB_PHOTOS);
                break;

                case FilterPhotosWithRole::Role::GroupRepresentative:
                    m_filterResult = QString("SELECT groups.representative_id FROM groups");
                break;

                case FilterPhotosWithRole::Role::GroupMember:
                    m_filterResult = QString("SELECT groups_members.photo_id FROM groups_members");
                break;
            }
        }

        void visit(Database::FilterPhotosWithPerson* personFilter) override
        {
            m_filterResult = QString("SELECT photos.id FROM %1 JOIN (%2) ON (%2.photo_id = %1.id) WHERE %2.person_id = '%3'")
                                .arg(TAB_PHOTOS)
                                .arg(TAB_PEOPLE)
                                .arg(personFilter->person_id);
        }

        QString m_filterResult;
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
        QString result;

        QStringList filters_data;

        for (const IFilter::Ptr& filter: filters)
        {
            const QString currentfilterData = visitor.visit(filter);
            filters_data.append(currentfilterData);
        }

        if (filters_data.empty())
            result = QString("SELECT id FROM %1").arg(TAB_PHOTOS);
        else if (filters_data.size() == 1)
            result = filters_data.front();
        else
        {
            result = QString("SELECT id FROM %1 WHERE ").arg(TAB_PHOTOS);

            for(auto it = filters_data.begin(); it != filters_data.end(); ++it)
            {
                if (it->isEmpty())
                    continue;

                result += QString("id IN (%1)").arg(*it);

                const auto next = std::next(it);

                if (next != filters_data.end())
                    result += " AND ";
            }
        }

        return result;
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

}
