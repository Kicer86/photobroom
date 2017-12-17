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

    DatabaseMigrator::DatabaseMigrator(QSqlDatabase& db, ISqlQueryExecutor* executor): m_executor(executor), m_db(db)
    {

    }


    DatabaseMigrator::~DatabaseMigrator()
    {

    }


    bool DatabaseMigrator::needsMigration() const
    {
        int v = 0;
        bool status = fetchVersion(v);

        if (status)
            status = v < 1;

        return status;
    }


    bool DatabaseMigrator::migrate()
    {
        int v = 0;

        bool status = fetchVersion(v);

        if (status && v < 1)
            status = convertToV1();

        return status;
    }


    bool DatabaseMigrator::fetchVersion(int& v) const
    {
        QSqlQuery query(m_db);

        BackendStatus status = m_executor->exec("SELECT version FROM " TAB_VER ";", &query);

        if (status)
            status = query.next()? StatusCodes::Ok: StatusCodes::QueryFailed;

        if (status)
            v = query.value(0).toInt();

        return status;
    }


    bool DatabaseMigrator::convertToV1()
    {
        return true;
    }

}
