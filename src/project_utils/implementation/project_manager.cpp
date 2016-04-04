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

#include <cassert>

#include <QSettings>
#include <QFileInfo>
#include <QDir>

#include <core/base_tags.hpp>
#include <database/idatabase_builder.hpp>
#include <database/project_info.hpp>
#include <database/idatabase.hpp>
#include <plugins/iplugin_loader.hpp>
#include <system/system.hpp>

#include "project.hpp"

namespace
{
    struct StoreTag: Database::AStoreTagTask
    {
        virtual ~StoreTag() {}

        virtual void got(bool status)
        {
            assert(status);
        }
    };
}

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


ProjectInfo ProjectManager::new_prj(const QString& prjName, const Database::IPlugin* prjPlugin, const QString& location)
{
    QDir().mkpath(location);

    const QDir storagePath(location);
    const QString prjPath = storagePath.absoluteFilePath( prjName + ".bpj");
    const ProjectInfo prjInfo(prjPath);
    const QString dbLocation = prjInfo.getInternalLocation() + "/db";

    QDir().mkpath(dbLocation);

    //prepare database
    Database::ProjectInfo dbPrjInfo = prjPlugin->initPrjDir(dbLocation, prjName);

    //prepare project file
    QSettings prjFile(prjPath, QSettings::IniFormat);

    prjFile.beginGroup("Database");
    prjFile.setValue("backend", dbPrjInfo.backendName);
    prjFile.setValue("location", dbPrjInfo.databaseLocation);
    prjFile.endGroup();

    prjFile.beginGroup("Project");
    prjFile.setValue("name", prjName);
    prjFile.endGroup();

    return prjInfo;
}


std::deque<ProjectInfo> ProjectManager::listProjects()
{
    std::deque<ProjectInfo> result;

    return result;
}


std::unique_ptr<Project> ProjectManager::open(const ProjectInfo& prjInfo, Database::IBuilder::OpenResult openResult)
{
    const QString& prjPath = prjInfo.getPath();

    QSettings prjFile(prjPath, QSettings::IniFormat);

    prjFile.beginGroup("Database");
    QString backend  = prjFile.value("backend").toString();
    QString location = prjFile.value("location").toString();
    prjFile.endGroup();

    Database::ProjectInfo dbPrjInfo(location, backend);
    auto db = m_dbBuilder->get(dbPrjInfo, openResult);

    Project* result = new Project(std::move(db), prjInfo);

    return std::unique_ptr<Project>(result);
}


bool ProjectManager::remove(const ProjectInfo& name)
{
    return true;
}


QString ProjectManager::getPrjStorage() const
{
    QString result;
    const QString path = System::getApplicationConfigDir();

    QDir basePath(path);

    if (basePath.exists("projects") == false)
        basePath.mkdir("projects");

    if (basePath.cd("projects"))
        result = basePath.absolutePath();

    return result;
}
