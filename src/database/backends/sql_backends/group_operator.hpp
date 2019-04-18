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

#ifndef GROUPOPERATOR_HPP
#define GROUPOPERATOR_HPP

#include <QString>

#include <database/igroup_operator.hpp>

namespace Database
{
    struct IBackend;
    struct IGenericSqlQueryGenerator;
    struct ISqlQueryExecutor;

    class GroupOperator: public IGroupOperator
    {
        public:
            GroupOperator(const QString &, const IGenericSqlQueryGenerator *, Database::ISqlQueryExecutor *, IBackend *);

            Group::Id addGroup(const Photo::Id &, Group::Type) override final;
            Photo::Id removeGroup(const Group::Id &) override final;
            Group::Type type(const Group::Id &) const override final;

        private:
            QString m_connectionName;
            const IGenericSqlQueryGenerator* m_queryGenerator;
            ISqlQueryExecutor* m_executor;
            IBackend* m_backend;
    };
}

#endif // GROUPOPERATOR_HPP
