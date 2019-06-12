/*
 * Class reponsible for managing photo Change Log management
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

    QString encodeTag(const TagNameInfo& tagInfo, const TagValue& value)
    {
        const QString encoded = QString("%1 %2")
                                    .arg(tagInfo.getTag())
                                    .arg(encodeTag(value));

        return encoded;
    }

    QString encodeTag(const TagNameInfo& tagInfo, const TagValue& valueOld, const TagValue& valueNew)
    {
        const QString encoded = QString("%1 %2 %3")
                                    .arg(tagInfo.getTag())
                                    .arg(encodeTag(valueOld))
                                    .arg(encodeTag(valueNew));

        return encoded;
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

        DB_ERR_ON_FALSE(m_executor->exec(query));
    }


    void PhotoChangeLogOperator::process(const Photo::Id& id, const Tag::TagsList& oldTags, const Tag::TagsList& newTags) const
    {
        std::vector<std::pair<TagNameInfo, TagValue>> tagsRemoved;
        std::vector<std::tuple<TagNameInfo, TagValue, TagValue>> tagsChanged;
        std::vector<std::pair<TagNameInfo, TagValue>> tagsAdded;

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
            const QString data = QString("%1 %2, %3 %4")
                                    .arg(oldGroupInfo.group_id)
                                    .arg(newGroupInfo.group_id)
                                    .arg(oldGroupInfo.role)
                                    .arg(newGroupInfo.role);

            append(id, Modify, Group, data);
        }
        else if (oldGroupInfo.group_id.valid() && !newGroupInfo.group_id)     // only old valid -> removal
        {
            const QString data = QString("%1, %3 %4")
                                    .arg(oldGroupInfo.group_id)
                                    .arg(oldGroupInfo.role)
                                    .arg(newGroupInfo.role);

            append(id, Remove, Group, data);
        }
        else if (!oldGroupInfo.group_id && newGroupInfo.group_id.valid())    // only new valid -> addition
        {
            const QString data = QString("%1, %3 %4")
                                    .arg(newGroupInfo.group_id)
                                    .arg(oldGroupInfo.role)
                                    .arg(newGroupInfo.role);

            append(id, Add, Group, data);
        }
    }

}
