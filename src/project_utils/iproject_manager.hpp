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

#include <database/idatabase_plugin.hpp>

class QString;

struct IProject;

struct ProjectInfo
{
    ProjectInfo(const QString& _name, const QString& _id): name(_name), id(_id) {}
    ProjectInfo(): name(), id() {}

    bool isValid() const
    {
        return name.isEmpty() == false && id.isEmpty() == false;
    }

    const QString& getName() const
    {
        return name;
    }

    const QString& getId() const
    {
        return id;
    }

    private:
        QString name;
        QString id;
};

struct IProjectManager
{
    virtual ~IProjectManager() {}

    virtual ProjectInfo new_prj(const QString& name, const Database::IPlugin *) = 0;
    virtual std::deque<ProjectInfo> listProjects() = 0;
    virtual std::shared_ptr<IProject> open(const ProjectInfo &) = 0;
    virtual bool save(const IProject *) = 0;
    virtual bool remove(const ProjectInfo &) = 0;
};

#endif // PROJECTMANAGER_H
