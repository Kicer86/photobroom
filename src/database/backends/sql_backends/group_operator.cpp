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

#include "group_operator.hpp"

#include <QSqlDatabase>
#include <QSqlQuery>

#include <core/ilogger.hpp>

#include "database/ibackend.hpp"
#include "database/iphoto_change_log_operator.hpp"
#include "isql_query_constructor.hpp"
#include "isql_query_executor.hpp"
#include "query_structs.hpp"
#include "tables.hpp"

namespace Database
{

    GroupOperator::GroupOperator(const QString& name,
                                 const IGenericSqlQueryGenerator* generator,
                                 ISqlQueryExecutor* executor,
                                 ILogger* logger,
                                 IBackend* backend):
        m_connectionName(name),
        m_queryGenerator(generator),
        m_executor(executor),
        m_logger(logger),
        m_backend(backend)
    {
    }


    Group::Id GroupOperator::addGroup(const Photo::Id& id, Group::Type type)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        Group::Id grp_id;

        InsertQueryData insertData(TAB_GROUPS);

        insertData.setColumns("id", "representative_id", "type");
        insertData.setValues(InsertQueryData::Value::Null, id, static_cast<int>(type));

        QSqlQuery query = m_queryGenerator->insert(db, insertData);

        bool status = m_executor->exec(query);

        //update id
        if (status)                                    // Get Id from database after insert
        {
            QVariant group_id  = query.lastInsertId();
            status = group_id.isValid();

            if (status)
            {
                grp_id = Group::Id(group_id.toInt());

                m_backend->photoChangeLogOperator().groupCreated(grp_id, type, id);
                emit m_backend->photosModified( {id} );      // photo is now a representative  TODO: I don't like it. notifications about photos should not be raised from groups module
            }
        }

        return grp_id;
    }


    Photo::Id GroupOperator::removeGroup(const Group::Id& gid)
    {
        Photo::Id representativePhoto;
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        try
        {
            const QString query_str =
                QString("SELECT representative_id FROM %1 WHERE id=%2").arg(TAB_GROUPS).arg(gid);

            QSqlQuery query(db);
            DbErrorOnFalse(m_executor->exec(query_str, &query));
            DbErrorOnFalse(query.next());

            const Photo::Id ph_id( query.value(0).toInt() );

            const std::vector<Photo::Id> members = membersOf(gid);
            std::set<Photo::Id> modified_photos(members.cbegin(), members.cend());

            // add representative to modified_photos
            // as it won't be part of the group anymore
            modified_photos.insert(ph_id);

            DbErrorOnFalse(db.transaction());

            const QString members_delete =
                QString("DELETE FROM %1 WHERE group_id=%2").arg(TAB_GROUPS_MEMBERS).arg(gid);

            const QString group_delete =
                QString("DELETE FROM %1 WHERE id=%2").arg(TAB_GROUPS).arg(gid);

            DbErrorOnFalse(m_executor->exec(members_delete, &query));
            DbErrorOnFalse(m_executor->exec(group_delete, &query));

            DbErrorOnFalse(db.commit());

            // TODO: I don't like it. notifications about photos should not be raised from groups module
            m_backend->photoChangeLogOperator().groupDeleted(gid, ph_id, members);
            emit m_backend->photosModified(modified_photos);

            representativePhoto = ph_id;
        }
        catch(const db_error& ex)
        {
            db.rollback();

            m_logger->error(ex.what());
        }

        return representativePhoto;
    }


    Group::Type GroupOperator::type(const Group::Id& id) const
    {
        Group::Type type = Group::Invalid;

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        const QString query_str =
            QString("SELECT type FROM %1 WHERE id=%2").arg(TAB_GROUPS).arg(id);

        QSqlQuery query(db);
        bool status = m_executor->exec(query_str, &query);

        if (status && query.next())
        {
            const QVariant typeVariant = query.value(0);
            type = static_cast<Group::Type>(typeVariant.toInt());
        }

        return type;
    }


    std::vector<Photo::Id> GroupOperator::membersOf(const Group::Id& g_id) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        const QString members_list =
                QString("SELECT photo_id FROM %1 WHERE group_id=%2").arg(TAB_GROUPS_MEMBERS).arg(g_id);

        DbErrorOnFalse(m_executor->exec(members_list, &query));

        std::vector<Photo::Id> members;

        while(query.next())
        {
            // add members to modified photos
            // as they won't be part of the group anymore

            const Photo::Id mem_id(query.value(0).toInt());
            members.push_back(mem_id);
        }

        return members;
    }


    std::vector<Group::Id> GroupOperator::listGroups() const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        const QString groups_list =
                QString("SELECT id FROM %1").arg(TAB_GROUPS);

        DbErrorOnFalse(m_executor->exec(groups_list, &query));

        std::vector<Group::Id> groups;

        while(query.next())
        {
            const Group::Id grp_id(query.value(0).toInt());
            groups.push_back(grp_id);
        }

        return groups;
    }
}
