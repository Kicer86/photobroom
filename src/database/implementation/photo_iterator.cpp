/*
 * SQLDatabase iterator
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

#include "photo_iterator.hpp"

#include <QtSql/QSqlQuery>

namespace Database
{

    struct PhotoIterator::Impl
    {
        QSqlQuery m_query;
    };


    PhotoIterator::PhotoIterator(const QSqlQuery& query): m_impl(new Impl)
    {
        m_impl->m_query = query;

        //goto first item
        ++(*this);
    }


    PhotoIterator::PhotoIterator(): m_impl(new Impl)
    {

    }


    PhotoIterator::~PhotoIterator()
    {

    }


    PhotoIterator::operator bool ()
    {
        const bool result = m_impl->m_query.isValid();
        return result;
    }


    bool PhotoIterator::operator!()
    {
        const bool result = m_impl->m_query.isValid();
        return !result;
    }


    PhotoIterator& PhotoIterator::operator++()
    {
        m_impl->m_query.next();
    }


    PhotoIterator PhotoIterator::operator++(int)
    {
        PhotoIterator other;

        ++(*this);
        return other;
    }

}
