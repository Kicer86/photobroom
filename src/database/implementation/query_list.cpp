/*
 * Photo Broom - photos management tool.
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

#include "query_list.hpp"

namespace Database
{

    QueryList::QueryList(): m_query()
    {

    }


    QueryList::QueryList(const InterfaceContainer<IQuery>& query): m_query(query)
    {

    }


    QueryList::~QueryList()
    {

    }


    PhotoIterator QueryList::begin() const
    {
        return PhotoIterator(m_query);     //return iterator feeded with query
    }


    PhotoIterator QueryList::end() const
    {
        return PhotoIterator();            //return invalid iterator
    }


    PhotoIterator QueryList::find() const
    {
        return PhotoIterator();
    }


    int QueryList::size() const
    {
        return m_query->size();
    }

}
