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

    }

}
