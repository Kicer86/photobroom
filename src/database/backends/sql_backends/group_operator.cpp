/*
 * Class for performing operations on groups
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

#include "database/ibackend.hpp"
#include "isql_query_constructor.hpp"
#include "isql_query_executor.hpp"
#include "query_structs.hpp"
#include "tables.hpp"

namespace Database
{

    GroupOperator::GroupOperator(const QString& name,
                                 const IGenericSqlQueryGenerator* generator,
                                 ISqlQueryExecutor* executor,
                                 IBackend* backend):
        m_connectionName(name),
        m_queryGenerator(generator),
        m_executor(executor),
        m_backend(backend)
    {
    }


    Group::Id GroupOperator::addGroup(const Photo::Id& id, GroupInfo::Type type)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        Group::Id grp_id;

        InsertQueryData insertData(TAB_GROUPS);

        insertData.setColumns("id", "representative_id", "type");
        insertData.setValues(InsertQueryData::Value::Null, id, static_cast<int>(type));

        QSqlQuery query = m_queryGenerator->insert(db, insertData);

        bool status = m_executor->exec(query);

        //update id
        if (status)                                    //Get Id from database after insert
        {
            QVariant group_id  = query.lastInsertId(); //TODO: WARNING: may not work (http://qt-project.org/doc/qt-5.1/qtsql/qsqlquery.html#lastInsertId)
            status = group_id.isValid();

            if (status)
                grp_id = Group::Id(group_id.toInt());

            emit m_backend->photoModified(id);        // photo is now a representative
        }

        return grp_id;
    }


    Photo::Id Database::GroupOperator::removeGroup(const Group::Id)
    {
        Photo::Id representativePhoto;
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        const QString query_str =
            QString("SELECT id, representative_id FROM %1").arg(TAB_GROUPS);

        QSqlQuery query(db);
        bool status = m_executor->exec(query_str, &query);

        if (status && query.next())
        {
            const Group::Id g_id( query.value(0).toInt() );
            const Photo::Id ph_id( query.value(1).toInt() );

            status = db.transaction();

            if (status)
            {
                const QString members_delete =
                    QString("DELETE FROM %1 WHERE group_id=%2").arg(TAB_GROUPS_MEMBERS).arg(g_id);

                const QString group_delete =
                    QString("DELETE FROM %1 WHERE id=%2").arg(TAB_GROUPS).arg(g_id);

                status = m_executor->exec(members_delete, &query) &&
                         m_executor->exec(group_delete, &query);

                if (status)
                    status = db.commit();
                else
                    status = db.rollback();

                if (status)
                    representativePhoto = ph_id;
            }
        }

        return representativePhoto;
    }

}
