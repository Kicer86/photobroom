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

#include <memory>

#include <QString>

#include <core/file_lock.hpp>

#include "project_utils_export.h"


namespace Database
{
    struct IDatabase;
}


struct PROJECT_UTILS_EXPORT ProjectInfo
{
        ProjectInfo(const QString& path);
        ProjectInfo();

        bool isValid() const;

        const QString& getPath() const;
        const QString& getBaseDir() const;
        const QString& getName() const;
        const QString& getInternalLocation() const;     // subdirectory of baseDir for internal Photo Broom files (database, log files, etc)

    private:
        QString path;
        QString baseDir;
        QString name;
        QString internalLocation;
};


class PROJECT_UTILS_EXPORT Project
{
    public:
        Project(std::unique_ptr<Database::IDatabase> &&, const ProjectInfo &);
        Project(const Project &) = delete;
        virtual ~Project();

        Project& operator=(const Project &) = delete;

        bool lockProject();
        QString insertExternalPhoto(const QString &);         // copies given photo from external location to project's internal location

        Database::IDatabase* getDatabase() const;
        const ProjectInfo& getProjectInfo() const;
        QString makePathRelative(const QString &) const;

    private:
        ProjectInfo m_prjInfo;
        std::unique_ptr<Database::IDatabase> m_database;
        FileLock m_lock;
};

#endif // PROJECT_HPP
