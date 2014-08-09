/*
 * Implementation of IQuery for SQL databases.
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

#ifndef SQLDBQUERY_H
#define SQLDBQUERY_H

#include <QSqlQuery>

#include <database/photo_iterator.hpp>

namespace Database
{

    struct SqlDBQuery: IQuery
    {
        SqlDBQuery(const QSqlQuery& query, IBackend* ibackend): m_query(query), m_backend(ibackend) {}
        SqlDBQuery(const SqlDBQuery &) = delete;
        SqlDBQuery& operator=(const SqlDBQuery &) = delete;

        virtual IQuery* clone() const;
        virtual PhotoInfo::Id getId() const;
        virtual bool gotoNext();
        virtual bool valid() const;
        virtual int size() const;
        virtual IBackend* backend() const;

        QSqlQuery m_query;
        IBackend* m_backend;
    };

}

#endif // SQLDBQUERY_H
