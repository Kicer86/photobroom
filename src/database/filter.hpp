/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef FILTER_HPP
#define FILTER_HPP

#include <memory>
#include <vector>
#include <variant>

#include <QString>

#include <core/data_ptr.hpp>
#include <core/tag.hpp>
#include <core/search_expression_evaluator.hpp>

#include "database_export.h"
#include "iphoto_info.hpp"
#include "person_data.hpp"

namespace Database
{
    //filters

    struct EmptyFilter;
    struct GroupFilter;
    struct FilterPhotosWithTag;
    struct FilterPhotosWithFlags;
    struct FilterPhotosWithSha256;
    struct FilterNotMatchingFilter;
    struct FilterPhotosWithId;
    struct FilterPhotosMatchingExpression;
    struct FilterPhotosWithPath;
    struct FilterPhotosWithRole;
    struct FilterPhotosWithPerson;
    struct FilterPhotosWithGeneralFlag;


    typedef std::variant<EmptyFilter,
                         GroupFilter,
                         FilterPhotosWithTag,
                         FilterPhotosWithFlags,
                         FilterPhotosWithSha256,
                         FilterNotMatchingFilter,
                         FilterPhotosWithId,
                         FilterPhotosMatchingExpression,
                         FilterPhotosWithPath,
                         FilterPhotosWithRole,
                         FilterPhotosWithPerson,
                         FilterPhotosWithGeneralFlag
    > Filter;

    enum class ComparisonOp
    {
        Equal,
        Less,
        LessOrEqual,
        Greater,
        GreaterOrEqual,
    };

    enum class LogicalOp
    {
        And,
        Or,
    };

    struct DATABASE_EXPORT EmptyFilter
    {

    };

    struct DATABASE_EXPORT GroupFilter
    {
        GroupFilter(const std::vector<Filter> &);
        GroupFilter(const std::initializer_list<Filter> &);

        std::vector<Filter> filters;
    };

    struct DATABASE_EXPORT FilterPhotosWithTag
    {
        TagTypes tagType;
        TagValue tagValue;
        ComparisonOp valueMode;
        bool includeEmpty;

        FilterPhotosWithTag(const TagTypes &, const TagValue & = TagValue(), ComparisonOp = ComparisonOp::Equal, bool include_empty = false);
    };

    struct DATABASE_EXPORT FilterPhotosWithFlags
    {
        FilterPhotosWithFlags();
        FilterPhotosWithFlags(const std::map<Photo::FlagsE, int> &);

        ComparisonOp comparisonMode(Photo::FlagsE) const;

        std::map<Photo::FlagsE, int> flags;
        std::map<Photo::FlagsE, ComparisonOp> comparison;
        LogicalOp mode;
    };

    struct DATABASE_EXPORT FilterPhotosWithSha256
    {
        FilterPhotosWithSha256();

        Photo::Sha256sum sha256;
    };

    struct DATABASE_EXPORT FilterNotMatchingFilter
    {
        FilterNotMatchingFilter(const Filter &);

        ol::data_ptr<Filter> filter;
    };

    struct DATABASE_EXPORT FilterPhotosWithId
    {
        FilterPhotosWithId();

        Photo::Id filter;
    };

    struct DATABASE_EXPORT FilterPhotosMatchingExpression
    {
        FilterPhotosMatchingExpression(const SearchExpressionEvaluator::Expression &);

        SearchExpressionEvaluator::Expression expression;
    };

    struct DATABASE_EXPORT FilterPhotosWithPath
    {
        explicit FilterPhotosWithPath(const QString &);

        QString path;
    };

    struct DATABASE_EXPORT FilterPhotosWithRole
    {
        enum class Role
        {
            Regular,
            GroupRepresentative,
            GroupMember,
        };

        explicit FilterPhotosWithRole(Role);

        Role m_role;
    };

    struct DATABASE_EXPORT FilterPhotosWithPerson
    {
        explicit FilterPhotosWithPerson(const Person::Id &);

        Person::Id person_id;
    };

    struct DATABASE_EXPORT FilterPhotosWithGeneralFlag
    {
        explicit FilterPhotosWithGeneralFlag(const QString& name, int value);

        QString name;
        int value;
    };
}

#endif // FILTER_HPP
