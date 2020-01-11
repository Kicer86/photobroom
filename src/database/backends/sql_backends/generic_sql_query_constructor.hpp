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

#ifndef GENERICSQLQUERYCONSTRUCTOR_HPP
#define GENERICSQLQUERYCONSTRUCTOR_HPP

#include "isql_query_constructor.hpp"

#include "sql_backend_base_export.h"

namespace Database
{

    class SQL_BACKEND_BASE_EXPORT GenericSqlQueryConstructor: public IGenericSqlQueryGenerator
    {
        public:
            GenericSqlQueryConstructor();
            GenericSqlQueryConstructor(const GenericSqlQueryConstructor &) = delete;
            ~GenericSqlQueryConstructor();

            GenericSqlQueryConstructor& operator=(const GenericSqlQueryConstructor &) = delete;

            QString prepareInsertQuery(const InsertQueryData &) const;
            QString prepareUpdateQuery(const UpdateQueryData &) const;

        protected:
            virtual QString prepareCreationQuery(const QString& name, const QString& columns) const override;
            virtual QString prepareFindTableQuery(const QString& name) const override;

            virtual QSqlQuery insert(const QSqlDatabase &, const InsertQueryData &) const override;
            virtual QSqlQuery update(const QSqlDatabase &, const UpdateQueryData &) const override;
            
        private:
            std::vector<QString> preparePlaceholders(const InsertQueryData &) const;
    };

}

#endif // GENERICSQLQUERYCONSTRUCTOR_HPP
