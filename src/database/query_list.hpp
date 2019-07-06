/*
 * List of photos returned by query.
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

#ifndef QUERYLIST_H
#define QUERYLIST_H

#include "photo_iterator.hpp"

#include "database_export.h"

namespace Database
{
    struct IQuery;

    class DATABASE_DEPRECATED_EXPORT QueryList final
    {
        public:
            QueryList();
            explicit QueryList(const InterfaceContainer<IQuery> &);
            QueryList(const QueryList &) = default;
            ~QueryList();
            QueryList& operator=(const QueryList &) = default;

            PhotoIterator begin() const;
            PhotoIterator end() const;
            PhotoIterator find() const;
            int size() const;

        private:
            InterfaceContainer<IQuery> m_query;
    };
}

#endif // QUERYLIST_H
