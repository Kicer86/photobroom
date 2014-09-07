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
#include <QFileInfo>

#include <core/iplugin_loader.hpp>

#include "project.hpp"
#include <database/idatabase_builder.hpp>


ProjectManager::ProjectManager(): m_dbBuilder(nullptr)
{

}


ProjectManager::~ProjectManager()
{

}


void ProjectManager::set(Database::IBuilder* builder)
{
    m_dbBuilder = builder;
}


bool ProjectManager::new_prj(const QString& prjPath, const Database::IPlugin* prjPlugin)
{
    const QFileInfo prjPathInfo(prjPath);
    const QString prjDir = prjPathInfo.absolutePath();

    //prepare database
    Database::ProjectInfo prjInfo = prjPlugin->initPrjDir(prjDir);

    //prepare project file
    QSettings prjFile(prjPath, QSettings::IniFormat);

    prjFile.beginGroup("Database");
    prjFile.setValue("backend", prjInfo.backendName);
    prjFile.setValue("location", prjInfo.databaseLocation);
    prjFile.endGroup();

    const QSettings::Status status = prjFile.status();

    return status == QSettings::NoError;
}


std::shared_ptr<IProject> ProjectManager::open(const QString& path)
{
    QSettings prjFile(path, QSettings::IniFormat);

    prjFile.beginGroup("Database");
    QString backend  = prjFile.value("backend").toString();
    QString location = prjFile.value("location").toString();
    prjFile.endGroup();

    QFileInfo fileInfo(location);
    if (fileInfo.isRelative())
    {
        const QFileInfo prjFileInfo(path);
        const QString prjDir = prjFileInfo.absolutePath();

        location = prjDir + "/" + location;

        const QFileInfo locationInfo(location);
        location = locationInfo.absoluteFilePath();
    }
    else
        location = fileInfo.absoluteFilePath();  //cleanups

    auto result = std::make_shared<Project>();
    result->setPrjPath(path);
    result->setDBBackend(backend);
    result->setDBLocation(location);

    Database::ProjectInfo prjInfo(location, backend);
    Database::IDatabase* db = m_dbBuilder->get(prjInfo);

    result->setDatabase(db);

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

