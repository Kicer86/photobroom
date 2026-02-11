/*
 * Photo Broom - photos management tool.
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
#include <future>

#include <QSettings>
#include <QFileInfo>
#include <QDir>

#include <core/base_tags.hpp>
#include <database/idatabase_builder.hpp>
#include <database/project_info.hpp>
#include <database/idatabase.hpp>
#include <plugins/iplugin_loader.hpp>

#include "project.hpp"

import broom.system;


ProjectManager::ProjectManager(Database::IBuilder& builder): m_dbBuilder(builder)
{

}


ProjectInfo ProjectManager::new_prj(const QString& prjName, const Database::IPlugin* prjPlugin, const QString& location)
{
    QDir().mkpath(location);

    const QDir storagePath(location);
    const QString prjPath = storagePath.absoluteFilePath(prjName + ".bpj");
    const ProjectInfo prjInfo(prjPath);
    const QString dbDir = prjInfo.getInternalLocation(ProjectInfo::Database);

    QDir().mkpath(dbDir);

    //prepare database
    const Database::ProjectInfo dbPrjInfo = prjPlugin->initPrjDir(dbDir, prjName);

    // construct relative path to database location
    const QDir baseDir(prjInfo.getBaseDir());
    const QString databaseLocation = baseDir.relativeFilePath(dbPrjInfo.databaseLocation);

    //prepare project file
    QSettings prjFile(prjPath, QSettings::IniFormat);

    prjFile.beginGroup("Database");
    prjFile.setValue("backend", dbPrjInfo.backendName);
    prjFile.setValue("location", databaseLocation);
    prjFile.endGroup();

    prjFile.beginGroup("Project");
    prjFile.setValue("name", prjName);
    prjFile.endGroup();

    createIgnoreFile(prjInfo);

    return prjInfo;
}


ProjectManager::OpenStatus ProjectManager::open(const ProjectInfo& prjInfo)
{
    std::unique_ptr<Project> project = std::make_unique<Project>(m_dbBuilder, prjInfo);
    Database::BackendStatus db_status;

    const QString& prjPath = prjInfo.getPath();
    const bool prjFileExists = QFile::exists(prjPath);

    if (prjFileExists)
    {
        const QString basePath = prjInfo.getBaseDir();

        QSettings prjFile(prjPath, QSettings::IniFormat);

        prjFile.beginGroup("Database");
        const QString backend  = prjFile.value("backend").toString();
        const QString location = basePath + "/" + prjFile.value("location").toString();
        prjFile.endGroup();

        const bool lock_status = project->lockProject();

        if (lock_status)
        {
            std::promise<Database::BackendStatus> openResult;
            auto openFuture = openResult.get_future();

            const Database::ProjectInfo dbPrjInfo(location, backend);
            project->openDatabase(dbPrjInfo, [&openResult](const Database::BackendStatus& status)
            {
                openResult.set_value(status);
            });

            openFuture.wait();          // here we wait for result. If called from main thread, gui will be frozen until db gets open
            db_status = openFuture.get();

            createIgnoreFile(prjInfo);

        }
        else
            db_status = Database::StatusCodes::ProjectLocked;
    }
    else
        db_status = Database::StatusCodes::OpenFailed;

    return std::make_pair(std::move(project), db_status);
}


ProjectManager::OpenStatus ProjectManager::open(const QString& prjPath)
{
    ProjectManager::OpenStatus status(nullptr, Database::StatusCodes::OpenFailed);
    const QFileInfo prjFileInfo(prjPath);

    if (prjFileInfo.exists())
    {
        const ProjectInfo prjName(prjPath);

        status = open(prjName);
    }

    return status;
}


void ProjectManager::createIgnoreFile(const ProjectInfo& prjInfo) const
{
    const QDir baseDir(prjInfo.getInternalLocation());
    QFile ignore(baseDir.absoluteFilePath(Project::ignoreFileName));
    ignore.open(QFile::WriteOnly);
}
