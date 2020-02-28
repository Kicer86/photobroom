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
#include <QSqlQuery>

#include "tables.hpp"

namespace Database
{
    PeopleInformationAccessor::PeopleInformationAccessor(const QString& connectionName, Database::ISqlQueryExecutor& queryExecutor)
        : m_connectionName(connectionName)
        , m_executor(queryExecutor)
    {

    }


    std::vector<Person::Fingerprint> PeopleInformationAccessor::fingerprintsFor(const Person::Id& id)
    {
        const QString sql_query = QString("SELECT fingerprint FROM %1 JOIN %2 ON %2.fingerprint_id = %1.id WHERE %2.person_id = %3")
                                    .arg(TAB_FACES_FINGERPRINTS)
                                    .arg(TAB_PEOPLE)
                                    .arg(id);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        m_executor.exec(sql_query, &query);

        std::vector<Person::Fingerprint> result;

        while(query.next())
        {
            const QByteArray raw_fingerprint = query.value(0).toByteArray();
            const QList<QByteArray> splitted = raw_fingerprint.split(' ');

            Person::Fingerprint fingerprint;
            fingerprint.reserve(splitted.size());

            for(const QByteArray& component: splitted)
                fingerprint.push_back(component.toDouble());

            result.push_back(fingerprint);
        }

        return result;
    }

}
