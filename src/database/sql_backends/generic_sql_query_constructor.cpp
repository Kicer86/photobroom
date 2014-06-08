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

#include "generic_sql_query_constructor.hpp"

#include <assert.h>

#include <QStringList>

#include "query_structs.hpp"

namespace Database
{

    GenericSqlQueryConstructor::GenericSqlQueryConstructor()
    {

    }


    GenericSqlQueryConstructor::~GenericSqlQueryConstructor()
    {

    }


    SqlQuery GenericSqlQueryConstructor::insert(const InsertQueryData& data)
    {
        QString result;

        const QStringList& columns = data.getColumns();
        const QStringList& values = data.getValues();

        result = "INSERT INTO %1(%2) VALUES(%3)";

        result = result.arg(data.getName());
        result = result.arg(columns.join(", "));
        result = result.arg(values.join(", "));

        return result;
    }


    SqlQuery GenericSqlQueryConstructor::update(const UpdateQueryData& data)
    {
        QString result;

        QStringList columns = data.getColumns();
        QStringList values = data.getValues();
        const std::pair<QString, QString>& key = data.getCondition();

        result = "UPDATE %1 SET %2 WHERE %3";
        result = result.arg(data.getName());

        QString assigments;
        assert(columns.size() == values.size());
        const int s = std::min(columns.size(), values.size());
        for(int i = 0; i < s; i++)
        {
            assigments += columns[i] + "=" + values[i];

            if (i + 1 < s)
                assigments += ", ";
        }

        const QString condition(key.first + "=" + key.second);

        result = result.arg(assigments);
        result = result.arg(condition);

        return result;
    }

}
