/*
 * Database migration tool
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "database_migrator.hpp"

#include <QSqlQuery>
#include <QVariant>

#include "database/database_status.hpp"

#include "isql_query_executor.hpp"
#include "tables.hpp"

namespace Database
{

    DatabaseMigrator::DatabaseMigrator(ISqlQueryExecutor* executor): m_executor(executor)
    {

    }


    DatabaseMigrator::~DatabaseMigrator()
    {

    }


    bool DatabaseMigrator::needsMigration(const QSqlDatabase& db) const
    {
        QSqlQuery query(db);

        BackendStatus status = m_executor->exec("SELECT version FROM " TAB_VER ";", &query);

        if (status)
            status = query.next()? StatusCodes::Ok: StatusCodes::QueryFailed;

        if (status)
        {
            const int v = query.value(0).toInt();

            //if (v == 0)
            //convertToV1();

            // More than we expect? Quit with error
            if (v > 1)
                status = StatusCodes::BadVersion;
        }

        return status;
    }


    bool DatabaseMigrator::migrate(const QSqlDatabase&)
    {
        return true;
    }

}
