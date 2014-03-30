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

namespace Database
{
    struct IQuery;

    class QueryList final
    {
        public:
            QueryList();
            QueryList(std::unique_ptr<IQuery> &&);
            QueryList(const QueryList& other);
            ~QueryList();
            QueryList& operator=(const QueryList& other);
            bool operator==(const QueryList& other);

            PhotoIterator begin();
            PhotoIterator end();
            PhotoIterator find();

        private:

    };
}

#endif // QUERYLIST_H
