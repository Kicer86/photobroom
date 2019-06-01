/*
 * Class reponsible for managing photo Change Log management
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "photo_change_log_operator.hpp"

#include <QSqlDatabase>
#include <QSqlQuery>

#include "query_structs.hpp"
#include "tables.hpp"
#include "isql_query_constructor.hpp"
#include "isql_query_executor.hpp"


namespace Database
{
    PhotoChangeLogOperator::PhotoChangeLogOperator(const QString& name,
                                    const IGenericSqlQueryGenerator* generator,
                                    ISqlQueryExecutor* executor,
                                    ILogger* logger,
                                    IBackend* backend):
            m_connectionName(name),
            m_queryGenerator(generator),
            m_executor(executor),
            m_logger(logger),
            m_backend(backend)
        {
        }


    PhotoChangeLogOperator::~PhotoChangeLogOperator()
    {

    }


    void PhotoChangeLogOperator::append(const Photo::Id& ph_id, PhotoChangeLogOperator::Operation op, PhotoChangeLogOperator::Field field, const QString& data)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        InsertQueryData insertData(TAB_PHOTOS_CHANGE_LOG);

        insertData.setColumns("id", "photo_id", "operation", "field", "data");
        insertData.setValues(InsertQueryData::Value::Null,
                             ph_id,
                             static_cast<int>(op),
                             static_cast<int>(field),
                             data);

        QSqlQuery query = m_queryGenerator->insert(db, insertData);

        bool status = m_executor->exec(query);
        assert(status);
    }

}
