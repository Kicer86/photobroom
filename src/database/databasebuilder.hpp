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

#include "database_export.h"

namespace Database
{

    struct IDatabase;
    struct IFrontend;

    //configuration keys for databases
    extern DATABASE_EXPORT const char* databaseLocation;

    struct DATABASE_EXPORT Builder
    {
            virtual ~Builder();

            static Builder* instance();

            void initConfig();
            IDatabase* get();                             //Returns global database.
            std::unique_ptr<IDatabase> getSynchronous();  //Returns synchronous version of database.

            void closeAll();

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;

            Builder();
            Builder& operator=(const Builder &) = delete;
    };
}

#endif // DATABASEBUILDER_HPP
