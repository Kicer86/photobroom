/*
 * Base for SQL-based backends
 * This class is meant to be included to each project using it.
 *
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

#ifndef ASQLBACKEND_H
#define ASQLBACKEND_H

#include "idatabase.hpp"

#include <memory>

class QSqlQuery;
class QSqlDatabase;

namespace Database
{
    class Entry;
}

//TODO: close in namespace?
class ASqlBackend: public Database::IBackend
{
    public:
        ASqlBackend();
        ASqlBackend(const ASqlBackend& other) = delete;
        virtual ~ASqlBackend();

        ASqlBackend& operator=(const ASqlBackend& other) = delete;
        bool operator==(const ASqlBackend& other) = delete;

        void closeConnections();

    protected:
        //will be called from init(). Prepare database here
        virtual bool prepareDB(QSqlDatabase*) = 0;

        // Create table with given name and columns decription.
        // It may be necessary for table to meet features:
        // - FOREIGN KEY
        //
        // More features may be added in future.
        virtual QString prepareCreationQuery(const QString& name, const QString& columns) const = 0;

        virtual bool assureTableExists(const QString&, const QString &) const;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;

        virtual bool init() override final;
        virtual bool store(const Database::Entry &) override final;

        bool checkStructure();
};

#endif // ASQLBACKEND_H

