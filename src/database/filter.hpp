/*
 * Stuff for defining filters and sorting order for database queries.
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

#ifndef FILTER_H
#define FILTER_H

#include <memory>
#include <vector>

#include <QString>

#include <OpenLibrary/utils/optional.hpp>

#include <core/tag.hpp>

#include "database_export.h"
#include "iphoto_info.hpp"

#define FILTER_COMMAND() virtual void visitMe(IFilterVisitor* visitor) override { visitor->visit(this); } //

namespace Database
{
    struct IFilterVisitor;
    struct EmptyFilter;
    struct FilterPhotosWithTag;
    struct FilterPhotosWithFlags;
    struct FilterPhotosWithSha256;
    struct FilterNotMatchingFilter;
    struct FilterPhotosWithId;
    struct FilterPhotosMatchingExpression;
    struct FilterPhotosWithPath;

    struct DATABASE_EXPORT IFilter
    {
        typedef std::shared_ptr<IFilter> Ptr;

        virtual ~IFilter();
        virtual void visitMe(IFilterVisitor *) = 0;
    };

    struct DATABASE_EXPORT IFilterVisitor
    {
        virtual ~IFilterVisitor();

        virtual void visit(EmptyFilter *) = 0;
        virtual void visit(FilterPhotosWithTag *) = 0;
        virtual void visit(FilterPhotosWithFlags *) = 0;
        virtual void visit(FilterPhotosWithSha256 *) = 0;
        virtual void visit(FilterNotMatchingFilter *) = 0;
        virtual void visit(FilterPhotosWithId *) = 0;
        virtual void visit(FilterPhotosMatchingExpression *) = 0;
        virtual void visit(FilterPhotosWithPath *) = 0;
    };

    //filters

    struct DATABASE_EXPORT EmptyFilter: IFilter
    {
        virtual ~EmptyFilter();

        FILTER_COMMAND();
    };

    struct DATABASE_EXPORT FilterPhotosWithTag: IFilter
    {
        virtual ~FilterPhotosWithTag();

        FILTER_COMMAND();

        const TagNameInfo tagName;
        const TagValue tagValue;

        FilterPhotosWithTag(const TagNameInfo &, const TagValue & = TagValue());
    };

    struct DATABASE_EXPORT FilterPhotosWithFlags: IFilter
    {
        FilterPhotosWithFlags();
        virtual ~FilterPhotosWithFlags();

        FILTER_COMMAND();

        enum class Mode
        {
            And,
            Or,
        };

        std::map<Photo::FlagsE, int> flags;
        Mode mode;
    };

    struct DATABASE_EXPORT FilterPhotosWithSha256: IFilter
    {
        FilterPhotosWithSha256();
        virtual ~FilterPhotosWithSha256();

        FILTER_COMMAND();

        Photo::Sha256sum sha256;
    };

    struct DATABASE_EXPORT FilterNotMatchingFilter: IFilter
    {
        FilterNotMatchingFilter();
        virtual ~FilterNotMatchingFilter();

        FILTER_COMMAND();

        IFilter::Ptr filter;
    };

    struct DATABASE_EXPORT FilterPhotosWithId: IFilter
    {
        FilterPhotosWithId();
        virtual ~FilterPhotosWithId();

        FILTER_COMMAND();

        Photo::Id filter;
    };

    struct DATABASE_EXPORT FilterPhotosMatchingExpression: IFilter
    {
        FilterPhotosMatchingExpression(const QString &);
        virtual ~FilterPhotosMatchingExpression();

        FILTER_COMMAND();

        const QString expression;
    };

    struct DATABASE_EXPORT FilterPhotosWithPath: IFilter
    {
        FilterPhotosWithPath(const QString &);
        virtual ~FilterPhotosWithPath();

        FILTER_COMMAND();

        const QString path;
    };

}
#endif // FILTER_HPP
