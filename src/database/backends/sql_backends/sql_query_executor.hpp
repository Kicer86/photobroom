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

#ifndef SQLQUERYEXECUTOR_HPP
#define SQLQUERYEXECUTOR_HPP

#include <thread>

#include "isql_query_executor.hpp"

struct ILogger;

namespace Database
{

    class SqlQueryExecutor final: public ISqlQueryExecutor
    {
        public:
            SqlQueryExecutor();
            SqlQueryExecutor(const SqlQueryExecutor &) = delete;
            ~SqlQueryExecutor();

            void set(ILogger *);
            void set(std::thread::id);

            SqlQueryExecutor& operator=(const SqlQueryExecutor &) = delete;

            BackendStatus prepare(const QString& query, QSqlQuery* result) const override;
            BackendStatus exec(const std::vector<QString>& query, QSqlQuery* result) const override;
            BackendStatus exec(const QString& query, QSqlQuery* result) const override;
            BackendStatus exec(QSqlQuery& query) const override;

        private:
            std::thread::id m_database_thread_id;
            ILogger* m_logger;
    };

}

#endif // SQLQUERYEXECUTOR_H
