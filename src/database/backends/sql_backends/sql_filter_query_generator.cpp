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

    namespace
    {
        QString castedTagValue(const TagTypes& type)
        {
            if (type == Rating)
                return "CAST(tags.value AS INTEGER)";
            else
                return "tags.value";
        }

        QString comparisonString(ComparisonOp mode)
        {
            QString comparisonType;

            switch (mode)
            {
                case ComparisonOp::Greater:        comparisonType = ">";  break;
                case ComparisonOp::GreaterOrEqual: comparisonType = ">="; break;
                case ComparisonOp::LessOrEqual:    comparisonType = "<="; break;
                case ComparisonOp::Less:           comparisonType = "<";  break;
                case ComparisonOp::Equal:          comparisonType = "=";  break;
            }

            return comparisonType;
        }

        QString logicalString(LogicalOp mode)
        {
            QString logicalType;

            switch (mode)
            {
                case LogicalOp::And:        logicalType = "AND";  break;
                case LogicalOp::Or:         logicalType = "OR";  break;
            }

            return logicalType;
        }
    }

    SqlFilterQueryGenerator::SqlFilterQueryGenerator()
    {

    }


    SqlFilterQueryGenerator::~SqlFilterQueryGenerator()
    {

    }


    QString SqlFilterQueryGenerator::generate(const Filter& filter) const
    {
        const QString result = std::visit([this](const auto& arg) -> QString {
                return this->visit(arg);
            },
            filter
        );

        return result;
    }


    QString SqlFilterQueryGenerator::getFlagName(Photo::FlagsE flag) const
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


    QString SqlFilterQueryGenerator::visit(const EmptyFilter &) const
    {
        return QString("SELECT id FROM %1").arg(TAB_PHOTOS);
    }

    QString SqlFilterQueryGenerator::visit(const GroupFilter& groupFilter) const
    {
        QStringList filters_data;
        QString result;

        for (const Filter& filter: groupFilter.filters)
        {
            const QString currentfilterData = generate(filter);
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
                    result += " " + logicalString(groupFilter.mode) + " ";
            }
        }

        return result;
    }

    QString SqlFilterQueryGenerator::visit(const FilterPhotosWithTag& desciption) const
    {
        QString result;
        QString condition;
        const QString comparisonType = comparisonString(desciption.valueMode);

        if (desciption.tagValue.type() != Tag::ValueType::Empty)
        {
            // if we need to include empty (NULL) tag values, we need to
            // do 2 things: first is to convert any NULL occurence of tag value
            // into an empty string. Second is to include empty tag values in
            // query.
            if (desciption.includeEmpty)
            {
                condition = QString("COALESCE(%1.value, '') %3 '%2'")
                                    .arg(TAB_TAGS)
                                    .arg(desciption.tagValue.rawValue())
                                    .arg(comparisonType);
            }
            else
            {
                    condition = QString("%4.name = '%1' AND %5 %3 '%2'")
                                    .arg(desciption.tagType)
                                    .arg(desciption.tagValue.rawValue())
                                    .arg(comparisonType)
                                    .arg(TAB_TAGS)
                                    .arg(castedTagValue(desciption.tagType));
            }
        }
        else
            condition = QString(TAB_TAGS ".name = '%1'").arg(desciption.tagType);

        if (desciption.includeEmpty)
        {
            // here we include NULL tag values when `includeEmpty` is set
            result = QString("SELECT %1.id FROM %1 LEFT JOIN (%2) ON (%2.photo_id = %1.id AND %2.name = %4) WHERE %3")
                                .arg(TAB_PHOTOS)
                                .arg(TAB_TAGS)
                                .arg(condition)
                                .arg(desciption.tagType);
        }
        else
        {
            result = QString("SELECT %1.id FROM %1 JOIN (%2) ON (%2.photo_id = %1.id) WHERE %3")
                                .arg(TAB_PHOTOS)
                                .arg(TAB_TAGS)
                                .arg(condition);
        }

        return result;
    }

    QString SqlFilterQueryGenerator::visit(const FilterPhotosWithFlags& flags) const
    {
        QStringList conditions;

        for(const auto& it: flags.flags)
        {
            const QString flagName = getFlagName(it.first);
            const int flagValue = it.second;
            const QString comparisonType = comparisonString(flags.comparisonMode(it.first));

            conditions.append(QString(TAB_FLAGS ".%1 %3 '%2'")
                                .arg(flagName)
                                .arg(flagValue)
                                .arg(comparisonType));
        }

        QString merged_conditions;

        switch (flags.mode)
        {
            case LogicalOp::And:
                merged_conditions = conditions.join(" AND ");
                break;

            case LogicalOp::Or:
                merged_conditions = "( " + conditions.join(" OR ") + " )";
                break;
        }

        return QString("SELECT %1.id FROM %1 JOIN (%2) ON (%2.photo_id = %1.id) WHERE %3")
                .arg(TAB_PHOTOS)
                .arg(TAB_FLAGS)
                .arg(merged_conditions);
    }

    QString SqlFilterQueryGenerator::visit(const FilterPhotosWithSha256& sha256) const
    {
        assert(sha256.sha256.isEmpty() == false);

        return QString("SELECT id FROM %1 JOIN (%2) ON (%2.photo_id = %1.id) WHERE %2.sha256 = '%3'")
                .arg(TAB_PHOTOS)
                .arg(TAB_SHA256SUMS)
                .arg(sha256.sha256.constData());
    }

    QString SqlFilterQueryGenerator::visit(const FilterNotMatchingFilter& filter) const
    {
        const QString internal_condition = generate(*filter.filter.get());

        //http://stackoverflow.com/questions/367863/sql-find-records-from-one-table-which-dont-exist-in-another
        return QString("SELECT id FROM %1 WHERE id NOT IN (%2)")
                .arg(TAB_PHOTOS)
                .arg(internal_condition);
    }

    QString SqlFilterQueryGenerator::visit(const FilterPhotosWithId& filter) const
    {
        return QString("SELECT id FROM %1 WHERE id = '%2'")
                .arg(TAB_PHOTOS)
                .arg(filter.filter);
    }

    QString SqlFilterQueryGenerator::visit(const FilterPhotosMatchingExpression& filter) const
    {
        const SearchExpressionEvaluator::Expression conditions = filter.expression;
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

        return QString("SELECT %1.id FROM %1 WHERE %1.id IN (%2) OR %1.id IN (%3)")
                .arg(TAB_PHOTOS)
                .arg(tags_query)
                .arg(people_query);
    }

    QString SqlFilterQueryGenerator::visit(const FilterPhotosWithPath& filter) const
    {
        return QString("SELECT %1.id FROM %1 WHERE %1.path = '%2'")
                .arg(TAB_PHOTOS)
                .arg(filter.path);
    }

    QString SqlFilterQueryGenerator::visit(const FilterPhotosWithRole& filter) const
    {
        QString result;

        switch(filter.m_role)
        {
            case FilterPhotosWithRole::Role::Regular:
                result = QString("SELECT id FROM %1 WHERE id NOT IN "
                                        "("
                                            "SELECT groups_members.photo_id FROM groups_members "
                                            "UNION "
                                            "SELECT groups.representative_id FROM groups"
                                        ")")
                            .arg(TAB_PHOTOS);
            break;

            case FilterPhotosWithRole::Role::GroupRepresentative:
                result = QString("SELECT groups.representative_id FROM groups");
            break;

            case FilterPhotosWithRole::Role::GroupMember:
                result = QString("SELECT groups_members.photo_id FROM groups_members");
            break;
        }

        return result;
    }

    QString SqlFilterQueryGenerator::visit(const FilterPhotosWithPerson& personFilter) const
    {
        return QString("SELECT photos.id FROM %1 JOIN (%2) ON (%2.photo_id = %1.id) WHERE %2.person_id = '%3'")
                    .arg(TAB_PHOTOS)
                    .arg(TAB_PEOPLE)
                    .arg(personFilter.person_id);
    }

    QString SqlFilterQueryGenerator::visit(const FilterPhotosWithGeneralFlag& genericFlagsFilter) const
    {
        return QString("SELECT %1.id FROM %1 LEFT JOIN (%2) ON (%2.photo_id = %1.id AND %2.name = '%4') WHERE COALESCE(%2.value, 0) = %3")
                                .arg(TAB_PHOTOS)
                                .arg(TAB_GENERAL_FLAGS)
                                .arg(genericFlagsFilter.value)
                                .arg(genericFlagsFilter.name);
    }
}
