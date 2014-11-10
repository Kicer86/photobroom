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

#include <core/iplugin_loader.hpp>
#include <configuration/iconfiguration.hpp>
#include <configuration/constants.hpp>
#include <database/idatabase_builder.hpp>
#include <database/project_info.hpp>

#include "project.hpp"


ProjectManager::ProjectManager(): m_dbBuilder(nullptr), m_configuration(nullptr)
{

}


ProjectManager::~ProjectManager()
{

}


void ProjectManager::set(Database::IBuilder* builder)
{
    m_dbBuilder = builder;
}


void ProjectManager::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}


bool ProjectManager::new_prj(const QString& prjName, const Database::IPlugin* prjPlugin)
{
    QDir storagePath(getPrjStorage());
    storagePath.mkdir(prjName);
    storagePath.cd(prjName);
    
    const QString prjDir = storagePath.absolutePath();
    const QString prjPath = storagePath.absoluteFilePath("broom.bpj");

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


QStringList ProjectManager::listProjects()
{
    QStringList result;
    
    QString path = getPrjStorage();
    
    if (path.isEmpty() == false)
    {
        const QDir basePath(path);
        
        result = basePath.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    }
    
    return result;
}


std::shared_ptr<IProject> ProjectManager::open(const QString& prjName)
{
    QDir storagePath(getPrjStorage());
    storagePath.mkdir(prjName);
    storagePath.cd(prjName);

    const QString prjDir = storagePath.absolutePath();
    const QString prjPath = storagePath.absoluteFilePath("broom.bpj");

    QSettings prjFile(prjPath, QSettings::IniFormat);

    prjFile.beginGroup("Database");
    QString backend  = prjFile.value("backend").toString();
    QString location = prjFile.value("location").toString();
    prjFile.endGroup();

    auto result = std::make_shared<Project>();
    result->setPrjPath(prjPath);
    result->setDBBackend(backend);
    result->setDBLocation(location);

    Database::ProjectInfo prjInfo(location, backend, prjDir);
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


bool ProjectManager::remove(const QString& name)
{
    QDir storagePath(getPrjStorage());

    bool status = storagePath.exists(name);

    if (status)
        status = storagePath.remove(name);

    return status;
}


QString ProjectManager::getPrjStorage()
{
    QString result;
    auto path = m_configuration->findEntry(Configuration::BasicKeys::configLocation);
        
    if (path)
    {
        QDir basePath(*path);
        
        if (basePath.exists("projects") == false)
            basePath.mkdir("projects");
        
        if (basePath.cd("projects"))
            result = basePath.absolutePath();
    }
    else
        assert(!"Could not get configuration path");
   
    return result;
}

