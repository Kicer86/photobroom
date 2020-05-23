/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "photo_change_log_operator.hpp"

#include <QSqlDatabase>
#include <QSqlQuery>

#include "database/ibackend.hpp"
#include "query_structs.hpp"
#include "tables.hpp"
#include "isql_query_constructor.hpp"
#include "isql_query_executor.hpp"


namespace
{
    QString encodeTag(const TagValue& value)
    {
        const QString encodedValue = value.rawValue().toUtf8().toBase64();

        return encodedValue;
    }

    QString encodeTag(const TagTypeInfo& tagInfo, const TagValue& value)
    {
        const QString encoded = QString("%1 %2")
                                    .arg(tagInfo.getTag())
                                    .arg(encodeTag(value));

        return encoded;
    }

    QString encodeTag(const TagTypeInfo& tagInfo, const TagValue& valueOld, const TagValue& valueNew)
    {
        const QString encoded = QString("%1 %2 %3")
                                    .arg(tagInfo.getTag())
                                    .arg(encodeTag(valueOld))
                                    .arg(encodeTag(valueNew));

        return encoded;
    }

    QString decodeTag(const QString& encoded)
    {
        assert(encoded.split(" ").size() == 1);

        const QByteArray array = QByteArray::fromBase64(encoded.toUtf8());

        return array;
    }

    std::tuple<TagTypeInfo, TagValue> decodeTag2(const QString& encoded)
    {
        const QStringList items = encoded.split(" ");
        assert(items.size() == 2);

        const QString& tagInfoStr = items.front();
        const QString& tagValueStr = items.back();

        const TagTypes tagType = static_cast<TagTypes>(tagInfoStr.toInt());
        const TagTypeInfo decodedTagInfo = TagTypeInfo(tagType);
        const QString decodedTagValue = decodeTag(tagValueStr);

        return std::make_tuple(decodedTagInfo, decodedTagValue);
    }

    std::tuple<TagTypeInfo, TagValue, TagValue> decodeTag3(const QString& encoded)
    {
        const QStringList items = encoded.split(" ");
        assert(items.size() == 3);

        const QString& tagInfoStr = items[0];
        const QString& oldTagValueStr = items[1];
        const QString& newTagValueStr = items[2];

        const TagTypes tagType = static_cast<TagTypes>(tagInfoStr.toInt());
        const TagTypeInfo decodedTagInfo = TagTypeInfo(tagType);
        const QString decodedOldTagValue = decodeTag(oldTagValueStr);
        const QString decodedNewTagValue = decodeTag(newTagValueStr);

        return std::make_tuple(decodedTagInfo, decodedOldTagValue, decodedNewTagValue);
    }
}


namespace Database
{
    PhotoChangeLogOperator::PhotoChangeLogOperator(const QString& name,
                                    const IGenericSqlQueryGenerator* generator,
                                    const ISqlQueryExecutor* executor,
                                    ILogger* logger,
                                    IBackend* backend):
        m_connectionName(name),
        m_queryGenerator(generator),
        m_executor(executor),
        m_logger(logger),
        m_backend(backend)
    {
    }


    PhotoChangeLogOperator::~PhotoChangeLogOperator()
    {

    }


    QStringList PhotoChangeLogOperator::dumpChangeLog()
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        const QString queryStr = QString("SELECT photo_id, operation, field, data FROM %1").arg(TAB_PHOTOS_CHANGE_LOG);
        QSqlQuery query(queryStr, db);

        m_executor->exec(query);

        QStringList results;
        while(query.next())
        {
            const QVariant photoIdVar = query.value("photo_id");
            const QVariant operationVar = query.value("operation");
            const QVariant fieldVar = query.value("field");
            const QVariant dataVar = query.value("data");

            const int photoId = photoIdVar.toInt();
            const Operation operation = static_cast<Operation>(operationVar.toInt());
            const Field field = static_cast<Field>(fieldVar.toInt());
            const QString data = dataVar.toString();

            const QString result = format(photoId, operation, field, data);

            results.append(result);
        }

        return results;
    }


    void PhotoChangeLogOperator::append(const Photo::Id& ph_id, PhotoChangeLogOperator::Operation op, PhotoChangeLogOperator::Field field, const QString& data)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        InsertQueryData insertData(TAB_PHOTOS_CHANGE_LOG);

        insertData.setColumns("id", "photo_id", "operation", "field", "data", "date");
        insertData.setValues(InsertQueryData::Value::Null,
                             ph_id,
                             static_cast<int>(op),
                             static_cast<int>(field),
                             data,
                             InsertQueryData::Value::CurrentTime
                            );

        QSqlQuery query = m_queryGenerator->insert(db, insertData);

        DB_ERROR_ON_FALSE1(m_executor->exec(query));
    }

}
