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

#include "core/containers_utils.hpp"
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


    void PhotoChangeLogOperator::storeDifference(const Photo::Data& currentContent, const Photo::DataDelta& newContent)
    {
        assert(currentContent.id == newContent.getId());
        const Photo::Id& id = currentContent.id;

        if (newContent.has(Photo::Field::Tags))
        {
            const auto& oldTags = currentContent.tags;
            const auto& newTags = newContent.get<Photo::Field::Tags>();

            process(id, oldTags, newTags);
        }

        if (newContent.has(Photo::Field::GroupInfo))
        {
            const auto& oldGroupInfo = currentContent.groupInfo;
            const auto& newGroupInfo = newContent.get<Photo::Field::GroupInfo>();

            process(id, oldGroupInfo, newGroupInfo);
        }
    }


    void PhotoChangeLogOperator::groupCreated(const Group::Id& id, const Group::Type &, const Photo::Id& representative_id)
    {
        process(representative_id, GroupInfo(), GroupInfo(id, GroupInfo::Role::Representative));
    }


    void PhotoChangeLogOperator::groupDeleted(const Group::Id& id, const Photo::Id& representative, const std::vector<Photo::Id>& members)
    {
        process(representative, GroupInfo(id, GroupInfo::Role::Representative), GroupInfo());

        for(const Photo::Id& ph_id: members)
            process(ph_id, GroupInfo(id, GroupInfo::Role::Member), GroupInfo());
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

            const QString result = QString("photo id: %1. %2 %3. %4")
                                    .arg(photoId)
                                    .arg(fieldToStr(field))
                                    .arg(opToStr(operation))
                                    .arg(dataToStr(field, operation, data));

            results.append(result);
        }

        return results;
    }


    void PhotoChangeLogOperator::append(const Photo::Id& ph_id, PhotoChangeLogOperator::Operation op, PhotoChangeLogOperator::Field field, const QString& data) const
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


    void PhotoChangeLogOperator::process(const Photo::Id& id, const Tag::TagsList& oldTags, const Tag::TagsList& newTags) const
    {
        std::vector<std::pair<TagTypeInfo, TagValue>> tagsRemoved;
        std::vector<std::tuple<TagTypeInfo, TagValue, TagValue>> tagsChanged;
        std::vector<std::pair<TagTypeInfo, TagValue>> tagsAdded;

        compare(oldTags, newTags,
                std::back_inserter(tagsRemoved),
                std::back_inserter(tagsChanged),
                std::back_inserter(tagsAdded));

        for(const auto& d: tagsRemoved)
        {
            const QString data = encodeTag(d.first, d.second);
            append(id, Remove, Tags, data);
        }

        for(const auto& d: tagsChanged)
        {
            const QString data = encodeTag(std::get<0>(d), std::get<1>(d), std::get<2>(d));
            append(id, Modify, Tags, data);
        }

        for(const auto& d: tagsAdded)
        {
            const QString data = encodeTag(d.first, d.second);
            append(id, Add, Tags, data);
        }
    }


    void PhotoChangeLogOperator::process(const Photo::Id& id, const GroupInfo& oldGroupInfo, const GroupInfo& newGroupInfo) const
    {
        if (oldGroupInfo.group_id.valid() && newGroupInfo.group_id.valid())   // both valid -> modification
        {
            const QString data = QString("%1 %2,%3 %4")
                                    .arg(oldGroupInfo.group_id)
                                    .arg(newGroupInfo.group_id)
                                    .arg(oldGroupInfo.role)
                                    .arg(newGroupInfo.role);

            append(id, Modify, Group, data);
        }
        else if (oldGroupInfo.group_id.valid() && !newGroupInfo.group_id)     // only old valid -> removal
        {
            const QString data = QString("%1,%2")
                                    .arg(oldGroupInfo.group_id)
                                    .arg(oldGroupInfo.role);

            append(id, Remove, Group, data);
        }
        else if (!oldGroupInfo.group_id && newGroupInfo.group_id.valid())    // only new valid -> addition
        {
            const QString data = QString("%1,%2")
                                    .arg(newGroupInfo.group_id)
                                    .arg(newGroupInfo.role);

            append(id, Add, Group, data);
        }
    }


    QString PhotoChangeLogOperator::fieldToStr(Field field)
    {
        switch(field)
        {
            case Tags:  return "Tag";
            case Group: return "Group";
        }

        assert(!"unexpected");
        return "";
    }


    QString PhotoChangeLogOperator::opToStr(Operation op)
    {
        switch(op)
        {
            case Add:    return "added";
            case Modify: return "modified";
            case Remove: return "removed";
        }

        assert(!"unexpected");
        return "";
    }


    QString PhotoChangeLogOperator::dataToStr(PhotoChangeLogOperator::Field field,
                                              PhotoChangeLogOperator::Operation op,
                                              const QString& data)
    {
        QString result;

        switch(field)
        {
            case Tags:
                switch(op)
                {
                    case Add:
                    case Remove:
                    {
                        auto encoded = decodeTag2(data);
                        const TagTypeInfo& tag_info = std::get<0>(encoded);
                        const TagValue& tag_value = std::get<1>(encoded);

                        result = QString("%1: %2")
                                    .arg(tag_info.getName())
                                    .arg(tag_value.rawValue());
                    }
                    break;

                    case Modify:
                    {
                        auto encoded = decodeTag3(data);
                        const TagTypeInfo& tag_info = std::get<0>(encoded);
                        const TagValue& old_tag_value = std::get<1>(encoded);
                        const TagValue& new_tag_value = std::get<2>(encoded);

                        result = QString("%1: %2 -> %3")
                                    .arg(tag_info.getName())
                                    .arg(old_tag_value.rawValue())
                                    .arg(new_tag_value.rawValue());
                    }
                }
                break;

            case Group:
                switch(op)
                {
                    case Add:
                    case Remove:
                    {
                        const QStringList id_type = data.split(",");

                        result = QString("%1: %2")
                                    .arg(id_type[0])
                                    .arg(id_type[1]);
                    }
                    break;

                    case Modify:
                    {
                        const QStringList ids_types = data.split(",");
                        const QStringList ids = ids_types[0].split(" ");
                        const QStringList types = ids_types[1].split(" ");

                        result = QString("%1 -> %2, %3 -> %4")
                                    .arg(ids[0])
                                    .arg(ids[1])
                                    .arg(types[0])
                                    .arg(types[1]);
                    }
                }
                break;
        }

        return result;
    }

}
