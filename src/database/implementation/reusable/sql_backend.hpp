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

#include <memory>
#include <vector>

#include "database/idatabase.hpp"
#include "sql_backend_base_export.h"

class QSqlQuery;
class QSqlDatabase;

namespace Database
{
    class Entry;
    struct TableDefinition;

    class SQL_BACKEND_BASE_EXPORT ASqlBackend: public Database::IBackend
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

            //add tag to 'tags' table. The problem here is that tag names are unique. MERGE stetement should be used, but MySQL doesn't support it.
            //INSERT...ON DUPLICATE KEY UPDATE needs to be used there.
            //http://stackoverflow.com/questions/548541/insert-ignore-vs-insert-on-duplicate-key-update
            virtual bool addTag(const QString &) = 0;

            // Create table with given name and columns decription.
            // It may be necessary for table to meet features:
            // - FOREIGN KEY
            //
            // More features may be added in future.
            // Default implementation returns QString("CREATE TABLE %1(%2);").arg(name).arg(columnsDesc)
            virtual QString prepareCreationQuery(const QString& name, const QString& columns) const;

            //make sure table exists
            virtual bool assureTableExists(const TableDefinition &) const;

            //execute query. Functio for inheriting classes
            virtual bool exec(const QString &, QSqlQuery *) const;

        private:
            struct Data;
            std::unique_ptr<Data> m_data;

            virtual bool init() override final;
            virtual bool store(const Database::Entry &) override final;

            bool checkStructure();
            bool addDefaultTagsDefinitions();
            bool addDefaultTagsDefinitions(const std::vector<QString> &);
    };

}

#endif // ASQLBACKEND_H

