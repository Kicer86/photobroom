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

#include "generic_sql_query_constructor.hpp"

#include <assert.h>

#include <QStringList>
#include <QVariant>

#include "query_structs.hpp"

namespace Database
{
    template<typename T>
    QString join(T first, T last, const QString& spl)
    {
        QString result;

        for(; first != last;)
        {
            T current = first++;

            result += *current;

            if (first != last)
                result += spl;
        }

        return result;
    }


    QString convert(const InsertQueryData::Value& v)
    {
        QString result;

        switch (v)
        {
            case InsertQueryData::Value::CurrentTime:
                result = "CURRENT_TIMESTAMP";
                break;

            case InsertQueryData::Value::Null:
                result = "NULL";
                break;
        }

        return result;
    }


    GenericSqlQueryConstructor::GenericSqlQueryConstructor()
    {

    }


    GenericSqlQueryConstructor::~GenericSqlQueryConstructor()
    {

    }


    QString GenericSqlQueryConstructor::prepareInsertQuery(const InsertQueryData& data) const
    {
        QString result;

        const std::vector<QString>& columns = data.getColumns();
        const std::vector<QString> valuePlaceholders = preparePlaceholders(data);

        result = "INSERT INTO %1(%2) VALUES(%3)";

        result = result.arg(data.getName());
        result = result.arg(join(columns.begin(), columns.end(), ", "));
        result = result.arg(join(valuePlaceholders.begin(), valuePlaceholders.end(), ", "));

        return result;
    }


    QString GenericSqlQueryConstructor::prepareUpdateQuery(const UpdateQueryData& data) const
    {
        QString result;

        const std::vector<QString>& columns = data.getColumns();
        const std::vector<QString> valuePlaceholders = preparePlaceholders(data);
        const std::vector<std::pair<QString, QString>>& keys = data.getCondition();

        result = "UPDATE %1 SET %2 WHERE %3";
        result = result.arg(data.getName());

        QString assigments;
        assert(columns.size() == valuePlaceholders.size());
        const auto s = std::min(columns.size(), valuePlaceholders.size());

        for(std::size_t i = 0; i < s; i++)
        {
            assigments += columns[i] + "=" + valuePlaceholders[i];

            if (i + 1 < s)
                assigments += ", ";
        }

        QStringList conditions;

        for(const auto& key: keys)
        {
            const QString condition(key.first + "=:" + key.first);
            conditions.append(condition);
        }

        const QString condition = conditions.join(" AND ");

        result = result.arg(assigments);
        result = result.arg(condition);

        return result;
    }


    QString GenericSqlQueryConstructor::prepareCreationQuery(const QString& name, const QString& columns) const
    {
        return QString("CREATE TABLE %1(%2);").arg(name).arg(columns);
    }


    QString GenericSqlQueryConstructor::prepareFindTableQuery(const QString& name) const
    {
        return QString("SHOW TABLES LIKE '%1';").arg(name);
    }


    QSqlQuery GenericSqlQueryConstructor::insert(const QSqlDatabase& db, const InsertQueryData& data) const
    {
        const QString insertQuery = prepareInsertQuery(data);
        const std::vector<QString>& columns = data.getColumns();
        const std::vector<QVariant>& values = data.getValues();
        const std::size_t count = std::min(columns.size(), values.size());

        QSqlQuery query(db);
        query.prepare(insertQuery);

        for(std::size_t i = 0; i < count; i++)
            if (values[i].userType() != qMetaTypeId<InsertQueryData::Value>())
                query.bindValue(":" + columns[i], values[i]);

        return query;
    }


    QSqlQuery GenericSqlQueryConstructor::update(const QSqlDatabase& db, const UpdateQueryData& data) const
    {
        const QString updateQuery = prepareUpdateQuery(data);
        const std::vector<QString>& columns = data.getColumns();
        const std::vector<QVariant>& values = data.getValues();
        const std::size_t count = std::min(columns.size(), values.size());

        QSqlQuery query(db);
        query.prepare(updateQuery);

        for(std::size_t i = 0; i < count; i++)
            if (values[i].userType() != qMetaTypeId<InsertQueryData::Value>())
                query.bindValue(":" + columns[i], values[i]);

        const auto& keys = data.getCondition();
        for(const auto& key: keys)
            query.bindValue(":" + key.first, key.second);

        return query;
    }


    std::vector<QString> GenericSqlQueryConstructor::preparePlaceholders(const InsertQueryData& data) const
    {
        const std::vector<QString>& columns = data.getColumns();
        const std::vector<QVariant>& values = data.getValues();
        const std::size_t count = std::min(columns.size(), values.size());

        std::vector<QString> valuePlaceholders;
        valuePlaceholders.resize(count);

        for(std::size_t i = 0; i < count; i++)
        {
            const QVariant& value = values[i];

            if (value.userType() == qMetaTypeId<InsertQueryData::Value>())
                valuePlaceholders[i] = convert( value.value<InsertQueryData::Value>() );
            else
                valuePlaceholders[i] = ":" + columns[i];
        }

        return valuePlaceholders;
    }

}
