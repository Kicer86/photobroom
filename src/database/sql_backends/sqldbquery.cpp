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

#include "sqldbquery.hpp"
namespace Database
{
    
    Database::IQuery* SqlDBQuery::clone() const
    {
        IQuery* result = new SqlDBQuery(m_query, m_backend);
        return result;
    }


    QVariant SqlDBQuery::getField(IQuery::Fields name) const
    {
        QString nameStr;

        switch (name)
        {
            case IQuery::Fields::Id:
                nameStr = "photos_id";
                break;

            case IQuery::Fields::Hash:
                nameStr = "photos.hash";
                break;

            case IQuery::Fields::Path:
                nameStr = "photos.path";
                break;

            case IQuery::Fields::TagName:
                nameStr = "tag_names.name";
                break;

            case IQuery::Fields::TagValue:
                nameStr = "tags_value.value";
                break;

            case IQuery::Fields::TagType:
                nameStr = "tag_names.type";
                break;
        }

        return m_query.value(nameStr);
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

