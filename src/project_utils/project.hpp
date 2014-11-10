/*
 * Project data
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "iproject.hpp"

class Project: public IProject
{
    public:
        Project();
        Project(const Project &) = delete;
        virtual ~Project();

        Project& operator=(const Project &) = delete;

        //extra fields
        void setPrjPath(const QString &);

        //init fields
        void setDBBackend(const QString &);
        void setDBLocation(const QString &);
        void setDatabase(Database::IDatabase *);
        void setName(const QString &);

        // overrides
        QString getDBBackend() const override;
        QString getDBLocation() const override;
        QString getPrjPath() const override;
        Database::IDatabase* getDatabase() const override;
        QString getName() const override;

    private:
        QString m_backend;
        QString m_location;
        QString m_prjPath;
        QString m_name;
        Database::IDatabase* m_database;
};

#endif // PROJECT_HPP
