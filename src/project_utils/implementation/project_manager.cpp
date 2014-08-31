/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#include "project_manager.hpp"

#include <QSettings>

#include <core/iplugin_loader.hpp>

#include "project.hpp"


ProjectManager::ProjectManager(): m_pluginLoader(nullptr)
{

}


ProjectManager::~ProjectManager()
{

}


void ProjectManager::set(IPluginLoader* pluginLoader)
{
    m_pluginLoader = pluginLoader;
}


std::shared_ptr<IProject> ProjectManager::open(const QString& path)
{
    QSettings prjFile(path, QSettings::IniFormat);

    prjFile.beginGroup("Database");
    const QString backend  = prjFile.value("backend").toString();
    const QString location = prjFile.value("location").toString();
    prjFile.endGroup();

    auto result = std::make_shared<Project>();
    result->setPrjPath(path);
    result->setDBBackend(backend);
    result->setDBLocation(location);

    return result;
}


bool ProjectManager::save(const IProject* project)
{
    QSettings prjFile(project->getPrjPath(), QSettings::IniFormat);

    prjFile.beginGroup("Database");
    prjFile.setValue("backend",  project->getDBBackend());
    prjFile.setValue("location", project->getDBLocation());
    prjFile.endGroup();

    return true;
}

