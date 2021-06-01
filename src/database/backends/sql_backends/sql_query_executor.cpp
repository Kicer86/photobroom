/*
 * Photo Broom - photos management tool.
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


#include "sql_query_executor.hpp"

#include <cassert>
#include <thread>

#include <QMap>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <core/ilogger.hpp>

#include "isql_query_constructor.hpp"

namespace Database
{

    SqlQueryExecutor::SqlQueryExecutor(): m_database_thread_id(), m_logger(nullptr)
    {

    }


    SqlQueryExecutor::~SqlQueryExecutor()
    {

    }


    void SqlQueryExecutor::set(ILogger* logger)
    {
        m_logger = logger;
    }


    void SqlQueryExecutor::set(std::thread::id id)
    {
        m_database_thread_id = id;
    }


    BackendStatus SqlQueryExecutor::prepare(const QString& query, QSqlQuery* result) const
    {
        const BackendStatus status = result->prepare(query)? StatusCodes::Ok: StatusCodes::QueryPreparationFailed;

        return status;
    }


    BackendStatus SqlQueryExecutor::exec(QSqlQuery& query) const
    {
        // threads cannot be used with sql connections:
        // http://qt-project.org/doc/qt-5/threads-modules.html#threads-and-the-sql-module
        // make sure the same thread is used as at construction time.
        assert(std::this_thread::get_id() == m_database_thread_id);

        const auto start = std::chrono::steady_clock::now();
        const BackendStatus status = query.exec()? StatusCodes::Ok: StatusCodes::QueryFailed;
        const auto end = std::chrono::steady_clock::now();
        const auto diff = end - start;
        const auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
        const QString logMessage = QString("%1 Execution time: %2ms").arg(query.lastQuery()).arg(diff_ms);

        m_logger->trace(logMessage);

        if (status == false)
        {
            auto bound = query.boundValues();

            QString message = "Error: " + query.lastError().text() + " while performing query: " + query.lastQuery();

            if (bound.empty() == false)
            {
                message += " bound values:";
                for (auto it = bound.begin(); it != bound.end(); ++it)
                    message += " '" + it->toString() + "'";
            }

            qDebug() << message;
            m_logger->error(message);
        }

        assert(status);
        return status;
    }


    BackendStatus SqlQueryExecutor::exec(const QString& query, QSqlQuery* result) const
    {
        BackendStatus status = prepare(query, result);

        if (status)
            status = exec(*result);
        else
        {
            const QString message = QString("Error during query preparation. '%1' finished with: '%2'")
                                        .arg(query)
                                        .arg(result->lastError().text());

            m_logger->error(message);
        }

        return status;
    }


    BackendStatus SqlQueryExecutor::exec(const std::vector<QString>& queries, QSqlQuery* result) const
    {
        BackendStatus status(StatusCodes::Ok);

        for(size_t i = 0; i < queries.size() && status; i++)
            status = exec(queries[i], result);

        return status;
    }

}
