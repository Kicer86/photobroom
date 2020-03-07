/*
 * Copyright (C) 2020  Michał Walenciak <Kicer86@gmail.com>
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


#include "people_information_accessor.hpp"

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>

#include "isql_query_executor.hpp"
#include "isql_query_constructor.hpp"
#include "tables.hpp"
#include "query_structs.hpp"

namespace Database
{
    PeopleInformationAccessor::PeopleInformationAccessor(const QString& connectionName,
                                                         Database::ISqlQueryExecutor& queryExecutor,
                                                         const IGenericSqlQueryGenerator& query_generator)
        : m_connectionName(connectionName)
        , m_executor(queryExecutor)
        , m_query_generator(query_generator)
        , m_dbHasSizeFeature(false)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        m_dbHasSizeFeature = db.driver()->hasFeature(QSqlDriver::QuerySize);
    }


    std::vector<PersonName> PeopleInformationAccessor::listPeople()
    {
        const QString findQuery = QString("SELECT id, name FROM %1")
                                    .arg( TAB_PEOPLE_NAMES );

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        std::vector<PersonName> result;
        const bool status = m_executor.exec(findQuery, &query);

        if (status)
        {
            if (m_dbHasSizeFeature)
                result.reserve(static_cast<std::size_t>(query.size()));

            while(query.next())
            {
                const int id = query.value(0).toInt();
                const QString name = query.value(1).toString();
                const Person::Id pid(id);

                result.emplace_back(pid, name);
            }
        }

        return result;
    }


    std::vector<PersonInfo> PeopleInformationAccessor::listPeople(const Photo::Id& ph_id )
    {
        const QString findQuery = QString("SELECT %1.id, %1.person_id, %1.location, %1.fingerprint_id FROM %1 WHERE %1.photo_id = %2")
                                    .arg(TAB_PEOPLE)
                                    .arg(ph_id);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        std::vector<PersonInfo> result;
        const bool status = m_executor.exec(findQuery, &query);

        if (status)
        {
            if (m_dbHasSizeFeature)
                result.reserve(static_cast<std::size_t>(query.size()));

            while(query.next())
            {
                const int id_raw = query.value(0).toInt();
                const PersonInfo::Id id(id_raw);
                const Person::Id pid = query.isNull(1)?
                                           Person::Id():
                                           Person::Id(query.value(1).toInt());

                const PersonFingerprint::Id f_id = query.isNull(3)?
                            PersonFingerprint::Id():
                            PersonFingerprint::Id(query.value(3).toInt());

                QRect location;

                if (query.isNull(2) == false)
                {
                    const QVariant location_raw = query.value(2);
                    const QStringList location_list = location_raw.toString().split(QRegExp("[ ,x]"));
                    location = QRect(location_list[0].toInt(),
                                     location_list[1].toInt(),
                                     location_list[2].toInt(),
                                     location_list[3].toInt());
                }

                result.emplace_back(id, pid, ph_id, f_id, location);
            }
        }

        return result;
    }


    /**
     * \brief get person name for given person id
     */
    PersonName PeopleInformationAccessor::person(const Person::Id& p_id)
    {
        const QString findQuery = QString("SELECT id, name FROM %1 WHERE %1.id = %2")
                                    .arg( TAB_PEOPLE_NAMES )
                                    .arg(p_id);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        PersonName result;
        const bool status = m_executor.exec(findQuery, &query);

        if (status && query.next())
        {
            const int id = query.value(0).toInt();
            const QString name = query.value(1).toString();
            const Person::Id pid(id);

            result = PersonName (pid, name);
        }

        return result;
    }


    std::vector<PersonFingerprint> PeopleInformationAccessor::fingerprintsFor(const Person::Id& id)
    {
        const QString sql_query = QString("SELECT %1.id, fingerprint FROM %1 JOIN %2 ON %2.fingerprint_id = %1.id WHERE %2.person_id = %3")
                                    .arg(TAB_FACES_FINGERPRINTS)
                                    .arg(TAB_PEOPLE)
                                    .arg(id);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        m_executor.exec(sql_query, &query);

        std::vector<PersonFingerprint> result;

        while(query.next())
        {
            const PersonFingerprint::Id fid(query.value(0).toInt());
            const QByteArray raw_fingerprint = query.value(1).toByteArray();
            const QList<QByteArray> splitted = raw_fingerprint.split(' ');

            Person::Fingerprint fingerprint;
            fingerprint.reserve(splitted.size());

            for(const QByteArray& component: splitted)
                fingerprint.push_back(component.toDouble());

            result.emplace_back(fid, fingerprint);
        }

        return result;
    }


    std::map<PersonInfo::Id, PersonFingerprint> PeopleInformationAccessor::fingerprintsFor(const std::vector<PersonInfo::Id>& ids)
    {
        QStringList ids_list;
        for(const auto& id: ids)
            ids_list.append(QString::number(id));

        const QString sql_query = QString("SELECT %2.id, %1.id, fingerprint FROM %1 JOIN %2 ON %2.fingerprint_id = %1.id WHERE %2.id IN(%3)")
                                    .arg(TAB_FACES_FINGERPRINTS)
                                    .arg(TAB_PEOPLE)
                                    .arg(ids_list.join(","));

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        m_executor.exec(sql_query, &query);

        std::map<PersonInfo::Id, PersonFingerprint> result;

        while(query.next())
        {
            const PersonInfo::Id id(query.value(0).toInt());
            const PersonFingerprint::Id fid(query.value(1).toInt());
            const QByteArray raw_fingerprint = query.value(2).toByteArray();
            const QList<QByteArray> splitted = raw_fingerprint.split(' ');

            Person::Fingerprint fingerprint;
            fingerprint.reserve(splitted.size());

            for(const QByteArray& component: splitted)
                fingerprint.push_back(component.toDouble());

            result.emplace(id, PersonFingerprint(fid, fingerprint));
        }

        return result;
    }


    PersonFingerprint::Id PeopleInformationAccessor::store(const PersonFingerprint& fingerprint)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        QStringList fingerprint_list;
        for(double b: fingerprint.fingerprint())
            fingerprint_list.append(QString::number(b));

        const QString fingerprint_raw = fingerprint_list.join(" ");

        PersonFingerprint::Id fid = fingerprint.id();

        if (fid.valid())
        {
            if (fingerprint.fingerprint().empty())
            {
                const QString delete_query = QString ("DELETE from %1 WHERE id = %2")
                                                .arg(TAB_FACES_FINGERPRINTS)
                                                .arg(fid);

                QSqlQuery query(db);
                m_executor.exec(delete_query, &query);
            }
            else
            {
                UpdateQueryData updateData(TAB_FACES_FINGERPRINTS);
                updateData.addColumn("fingerprint");
                updateData.addValue(fingerprint_raw);
                updateData.addCondition("id", QString::number(fid));

                QSqlQuery query(db);
                query = m_query_generator.update(db, updateData);

                m_executor.exec(query);
            }
        }
        else
        {
            InsertQueryData insertData(TAB_FACES_FINGERPRINTS);
            insertData.addColumn("fingerprint");
            insertData.addValue(fingerprint_raw);

            QSqlQuery query(db);
            query = m_query_generator.insert(db, insertData);

            m_executor.exec(query);
            fid = PersonFingerprint::Id(query.lastInsertId().toInt());
        }

        return fid;
    }
}
