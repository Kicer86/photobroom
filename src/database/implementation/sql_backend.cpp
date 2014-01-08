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

namespace
{
    const char db_version[] = "0.1";
}

namespace Database
{

    struct ASqlBackend::Data
    {
        Data(): m_db() {}
        ~Data()
        {

        }

        QSqlDatabase m_db;

        bool exec(const QString& query, QSqlQuery* result) const
        {
            const bool status = result->exec(query);

            if (status == false)
                std::cerr << "SQLBackend: error: " << result->lastError().text().toStdString()
                          << " while performing query: " << query.toStdString() << std::endl;

            return status;
        }


        bool store(const Database::Entry& entry)
        {
            return false;
        }

    };


    ASqlBackend::ASqlBackend(): m_data(new Data)
    {

    }


    ASqlBackend::~ASqlBackend()
    {

    }


    void ASqlBackend::closeConnections()
    {
        if (m_data->m_db.isValid() && m_data->m_db.isOpen())
        {
            std::cout << "ASqlBackend: closing database connections." << std::endl;
            m_data->m_db.close();

            // Reset belowe is necessary.
            // There is a problem:when application is being closed, all qt resources (libraries etc) are being removed.
            // And it may happend that a driver for particular sql database will be removed before database is destroyed.
            // This will lead to crash as in database's destructor calls to driver are made.
            m_data.reset(nullptr);        //destroy database.
        }
    }


    QString ASqlBackend::prepareCreationQuery(const QString& name, const QString& columns) const
    {
        return QString("CREATE TABLE %1(%2);").arg(name).arg(columns);
    }


    bool ASqlBackend::assureTableExists(const QString &name, const QString &columnsDesc) const
    {
        QSqlQuery query(m_data->m_db);

        bool status = m_data->exec( QString("SHOW TABLES LIKE '%1';").arg(name), &query );

        //create table 'name' if doesn't exist
        bool empty = query.next() == false;
        if (status && empty)
            status = m_data->exec( prepareCreationQuery(name, columnsDesc), &query );

        return status;
    }


    bool ASqlBackend::init()
    {
        bool status = prepareDB(&m_data->m_db);

        if (status)
            status = m_data->m_db.open();

        if (status)
            status = checkStructure();
        else
            std::cerr << "SQLBackend: error opening database: " << m_data->m_db.lastError().text().toStdString() << std::endl;

        //TODO: crash when status == false;
        return status;
    }


    bool ASqlBackend::store(const Database::Entry& entry)
    {
        bool status = false;

        if (m_data)
            status = m_data->store(entry);
        else
            std::cerr << "ASqlBackend: database object does not exist." << std::endl;

        return status;
    }


    bool ASqlBackend::checkStructure()
    {
        //check if database 'broom' exists
        QSqlQuery query(m_data->m_db);
        bool status = m_data->exec("SHOW DATABASES LIKE 'broom';", &query);

        //create database broom if doesn't exists
        bool empty = query.next() == false;
        if (status && empty)
            status = m_data->exec("CREATE DATABASE IF NOT EXISTS `broom`;", &query);

        //use 'broom' database
        if (status)
            status = m_data->exec("USE broom;", &query);

        //check if table 'version_history' exists
        if (status)
            status = assureTableExists("version_history", "id INT AUTO_INCREMENT PRIMARY KEY, "
                                                          "version DECIMAL(4,2) NOT NULL, "    //xx.yy
                                                          "date TIMESTAMP NOT NULL");

        //at least one row must be present in table 'version_history'
        if (status)
            status = m_data->exec("SELECT COUNT(*) FROM version_history;", &query);

        if (status)
            status = query.next() == true;

        const QVariant rows = status? query.value(0): QVariant(0);

        //insert first entry
        if (status && rows == 0)
            status = m_data->exec(QString("INSERT INTO version_history(version, date)"
                                          " VALUES(%1, CURRENT_TIMESTAMP);")
                                         .arg(db_version), &query);

        //photos table
        if (status)
            status = assureTableExists("photos", "id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY, "
                                                 "store_date TIMESTAMP NOT NULL, "
                                                 "path VARCHAR(1024) NOT NULL");

        //tags table
        if (status)
            status = assureTableExists("tags", "tag_id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY, "
                                               "photo_id BIGINT UNSIGNED NOT NULL, "
                                               "FOREIGN KEY(photo_id) REFERENCES photos(id)");

        return status;
    }

}
