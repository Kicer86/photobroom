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

#include <core/tag.hpp>

#include "database_export.h"
#include "iphoto_info.hpp"

#define FILTER_COMMAND virtual void visitMe(IFilterVisitor* visitor) override { visitor->visit(this); }

namespace Database
{
    struct IFilterVisitor;
    struct FilterEmpty;
    struct FilterDescription;
    struct FilterFlags;
    struct FilterSha256;

    struct IFilter
    {
        typedef std::shared_ptr<IFilter> Ptr;

        virtual ~IFilter() {}
        virtual void visitMe(IFilterVisitor *) = 0;
    };

    struct DATABASE_EXPORT IFilterVisitor
    {
        virtual ~IFilterVisitor() {}

        virtual void visit(FilterEmpty *) = 0;
        virtual void visit(FilterDescription *) = 0;
        virtual void visit(FilterFlags *) = 0;
        virtual void visit(FilterSha256 *) = 0;
    };

    //filters

    struct DATABASE_EXPORT FilterEmpty: IFilter
    {
        virtual ~FilterEmpty() {}

        FILTER_COMMAND
    };

    struct DATABASE_EXPORT FilterDescription: IFilter
    {
        virtual ~FilterDescription() {}

        FILTER_COMMAND

        TagNameInfo tagName;
        QString tagValue;

        FilterDescription();
    };

    struct DATABASE_EXPORT FilterFlags: IFilter
    {
        FilterFlags();
        virtual ~FilterFlags() {}

        FILTER_COMMAND

        bool stagingArea;
    };

    struct DATABASE_EXPORT FilterSha256: IFilter
    {
        FilterSha256();
        virtual ~FilterSha256() {}

        FILTER_COMMAND

        IPhotoInfo::Hash sha256;
    };
}
#endif // FILTER_H
