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

class Project;
struct ProjectInfo;

struct IProjectManager
{
    typedef std::function<void(const Database::BackendStatus &)> OpenResult;

    virtual ~IProjectManager() = default;

    virtual ProjectInfo new_prj(const QString& name, const Database::IPlugin *, const QString& location) = 0;
    [[deprecated]] virtual std::deque<ProjectInfo> listProjects() = 0;
    virtual std::unique_ptr<Project> open(const ProjectInfo &, const OpenResult &) = 0;
    [[deprecated]] virtual bool remove(const ProjectInfo &) = 0;
};

#endif // PROJECTMANAGER_H
