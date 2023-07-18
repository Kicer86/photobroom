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

#ifndef PEOPLE_INFORMATION_ACCESSOR_HPP
#define PEOPLE_INFORMATION_ACCESSOR_HPP

#include <vector>

#include "database/apeople_information_accessor.hpp"

namespace Database
{
    struct ISqlQueryExecutor;
    struct IGenericSqlQueryGenerator;

    class PeopleInformationAccessor: public APeopleInformationAccessor
    {
        public:
            PeopleInformationAccessor(const QString &, Database::ISqlQueryExecutor &, const IGenericSqlQueryGenerator &);

            std::vector<PersonName>  listPeople() override final;
            std::vector<PersonInfo>  listPeople(const Photo::Id &) override final;
            PersonName               person(const Person::Id &) override final;
            std::vector<PersonFingerprint> fingerprintsFor(const Person::Id &) override;
            std::map<PersonInfo::Id, PersonFingerprint> fingerprintsFor(const std::vector<PersonInfo::Id>& id) override;
            Person::Id               store(const PersonName &) override final;
            PersonFingerprint::Id    store(const PersonFingerprint &) override;
            using APeopleInformationAccessor::store;

        private:
            const QString m_connectionName;
            Database::ISqlQueryExecutor& m_executor;
            const IGenericSqlQueryGenerator& m_query_generator;
            bool m_dbHasSizeFeature;

            void dropPersonInfo(const PersonInfo::Id &) override;
            PersonInfo::Id storePerson(const PersonInfo &) override;
            PersonName person(const QString &) const;
    };
}

#endif // PEOPLE_INFORMATION_ACCESSOR_HPP
