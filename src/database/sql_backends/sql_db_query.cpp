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

#include <QVariant>

#include "sql_db_query.hpp"
namespace Database
{

    Database::IQuery* SqlDBQuery::clone() const
    {
        IQuery* result = new SqlDBQuery(m_query, m_backend);
        return result;
    }


    PhotoInfo::Id SqlDBQuery::getId() const
    {
        static_assert(sizeof(PhotoInfo::Id::type) <= sizeof(int), "bad type !");   //TODO: watchout for type
        return PhotoInfo::Id(m_query.value("photos_id").toInt());
    }


    bool SqlDBQuery::gotoNext()
    {
        return m_query.next();
    }


    bool SqlDBQuery::valid() const
    {
        return m_query.isValid();
    }


    int SqlDBQuery::size() const
    {
        return m_query.size();
    }


    IBackend* SqlDBQuery::backend() const
    {
        return m_backend;
    }
}

