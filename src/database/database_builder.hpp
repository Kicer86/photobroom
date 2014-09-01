/*
    Builder for database.
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef DATABASEBUILDER_HPP
#define DATABASEBUILDER_HPP

#include <string>
#include <memory>

#include <QString>

#include "database_export.h"

struct IPluginLoader;
namespace Database
{

    struct ProjectInfo
    {
        QString projectPath;
        QString backendName;

        bool operator<(const ProjectInfo& other) const
        {
            bool status = false;

            if (projectPath < other.projectPath)
                status = true;
            else if (projectPath == other.projectPath && backendName < other.backendName)
                status = true;

            return status;
        }
    };

    struct IDatabase;
    struct IFrontend;

    //configuration keys for databases
    extern DATABASE_EXPORT const char* databaseLocation;

    struct DATABASE_EXPORT Builder
    {
            virtual ~Builder();

            static Builder* instance();

            void initConfig();
            void set(IPluginLoader *);

            IDatabase* get(const ProjectInfo &);

            void closeAll();

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;

            Builder();
            Builder& operator=(const Builder &) = delete;
    };
}

#endif // DATABASEBUILDER_HPP
