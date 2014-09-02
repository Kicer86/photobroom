/*
 *    Interface for database builder.
 *    Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef IDATABASEBUILDER_HPP
#define IDATABASEBUILDER_HPP

#include <string>
#include <memory>

#include <QString>

#include "database_export.h"

namespace Database
{
    struct ProjectInfo
    {
        QString databaseLocation;
        QString backendName;

        bool operator<(const ProjectInfo& other) const
        {
            bool status = false;

            if (databaseLocation < other.databaseLocation)
                status = true;
            else if (databaseLocation == other.databaseLocation && backendName < other.backendName)
                status = true;

            return status;
        }
    };

    struct IDatabase;

    struct IBuilder
    {
        virtual ~IBuilder() {}

        virtual IDatabase* get(const ProjectInfo &) = 0;
        virtual void closeAll() = 0;
    };
}

#endif // IDATABASEBUILDER_HPP
