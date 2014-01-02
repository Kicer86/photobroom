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

#include "sql_backend.hpp"

#include <iostream>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

ASqlBackend::ASqlBackend(): m_db(new QSqlDatabase)
{

}


ASqlBackend::~ASqlBackend()
{

}


void ASqlBackend::closeConnections()
{
    m_db->close();
}


bool ASqlBackend::init()
{
    bool status = prepareDB(m_db.get());

    if (status)
    {
        status = m_db->open();

        if (status)
        {
            QSqlQuery query(*m_db.get());
            status = exec("show databases like 'broom';", &query);

            if (status && query.next() == false)
                status = exec("create database if not exists `broom`;", &query);
        }
        else
            std::cerr << "SQLBackend: error opening database: " << m_db->lastError().text().toStdString() << std::endl;
    }

    return status;
}


bool ASqlBackend::store(const Database::Entry&)
{
    return false;
}


bool ASqlBackend::exec(const QString& query, QSqlQuery* result) const
{
    result->exec(query);

    const bool status = result->exec();

    if (status == false)
        std::cerr << "SQLBackend: error: " << result->lastError().text().toStdString()
                  << " while performing query: " << query.toStdString() << std::endl;

    return status;
}
