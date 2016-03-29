/*
 * Interface for project manager
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

#ifndef IPROJECTMANAGER_HPP
#define IPROJECTMANAGER_HPP

#include <memory>
#include <deque>

#include <QFileInfo>

#include <database/idatabase_plugin.hpp>

class QString;

struct IProject;


struct ProjectInfo
{
    ProjectInfo(const QString& _path): ProjectInfo()
    {
        const QFileInfo fi(_path);
        path = fi.absoluteFilePath();
        baseDir = fi.absolutePath();
        name = fi.baseName();
    }

    ProjectInfo(): path(), baseDir(), name() {}

    bool isValid() const
    {
        return path.isEmpty() == false;
    }

    const QString& getPath() const
    {
        return path;
    }

    const QString& getBaseDir() const
    {
        return baseDir;
    }

    const QString& getName() const
    {
        return name;
    }

    private:
        QString path;
        QString baseDir;
        QString name;
};


struct IProjectManager
{
    virtual ~IProjectManager();

    virtual ProjectInfo new_prj(const QString& name, const Database::IPlugin *, const QString& location) = 0;
    [[deprecated]] virtual std::deque<ProjectInfo> listProjects() = 0;
    virtual std::unique_ptr<IProject> open(const ProjectInfo &, Database::IBuilder::OpenResult) = 0;
    virtual bool save(const IProject *) = 0;
    [[deprecated]] virtual bool remove(const ProjectInfo &) = 0;
};

#endif // PROJECTMANAGER_H
