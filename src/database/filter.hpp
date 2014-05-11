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

#include "database_export.h"

#define FILTER_COMMAND virtual void visitMe(IFilterVisitor* visitor) { visitor->visit(this); }

namespace Database
{
    struct IFilterVisitor;
    struct FilterEmpty;
    struct FilterDescription;

    struct IFilter
    {
        typedef std::shared_ptr<IFilter> Ptr;

        virtual void visitMe(IFilterVisitor *) = 0;
    };

    struct DATABASE_EXPORT IFilterVisitor
    {
        virtual void visit(FilterEmpty *) = 0;
        virtual void visit(FilterDescription *) = 0;
    };

    struct FilterEmpty: IFilter
    {
        FILTER_COMMAND
    };

    struct DATABASE_EXPORT FilterDescription: IFilter
    {        
        FILTER_COMMAND

        QString tagName;
        QString tagValue;

        FilterDescription();
    };

}
#endif // FILTER_H
