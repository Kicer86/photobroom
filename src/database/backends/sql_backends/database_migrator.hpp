/*
 * Database migration tool
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef DATABASEMIGRATOR_HPP
#define DATABASEMIGRATOR_HPP


class QSqlDatabase;

namespace Database
{
    struct ISqlQueryExecutor;

    class DatabaseMigrator
    {
        public:
            DatabaseMigrator(ISqlQueryExecutor *);
            DatabaseMigrator(const DatabaseMigrator &) = delete;
            ~DatabaseMigrator();

            DatabaseMigrator& operator=(const DatabaseMigrator &) = delete;

            bool needsMigration(const QSqlDatabase &) const;
            bool migrate(const QSqlDatabase &);

        private:
            ISqlQueryExecutor* m_executor;
    };

}

#endif // DATABASEMIGRATOR_HPP
