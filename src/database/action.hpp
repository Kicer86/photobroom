/*
 * Actions to be performed on set of photos.
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef ACTION_HPP
#define ACTION_HPP

#include <memory>

#include "database_export.h"

#define ACTION_COMMAND virtual void visitMe(IActionVisitor* visitor) override { visitor->visit(this); }

namespace Database
{

    struct IActionVisitor;
    struct ModifyFlagAction;


    struct DATABASE_EXPORT IAction
    {
        typedef std::shared_ptr<IAction> Ptr;

        virtual ~IAction();
        virtual void visitMe(IActionVisitor *) = 0;
    };


    struct DATABASE_EXPORT IActionVisitor
    {
        virtual ~IActionVisitor();

        virtual void visit(ModifyFlagAction *) = 0;
    };
    

    struct DATABASE_EXPORT ModifyFlagAction: IAction
    {
        ModifyFlagAction();
        virtual ~ModifyFlagAction();

        ACTION_COMMAND
    };
}

#endif // ACTION_HPP
