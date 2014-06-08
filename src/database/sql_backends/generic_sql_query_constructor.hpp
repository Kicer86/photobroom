/*
 * Basic implementation of ISqlQueryConstructor interface
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

#ifndef GENERICSQLQUERYCONSTRUCTOR_H
#define GENERICSQLQUERYCONSTRUCTOR_H

#include "sql_query_constructor.hpp"

namespace Database
{

    class GenericSqlQueryConstructor : public ISqlQueryConstructor
    {
        public:
            GenericSqlQueryConstructor();
            GenericSqlQueryConstructor(const GenericSqlQueryConstructor &) = delete;
            ~GenericSqlQueryConstructor();

            GenericSqlQueryConstructor& operator=(const GenericSqlQueryConstructor &) = delete;

        protected:
            virtual SqlQuery insert(const InsertQueryData&);
            virtual SqlQuery update(const UpdateQueryData&);
    };

}

#endif // GENERICSQLQUERYCONSTRUCTOR_H
