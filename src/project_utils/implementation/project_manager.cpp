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
#include <core/base_tags.hpp>
#include <configuration/iconfiguration.hpp>
#include <configuration/constants.hpp>
#include <database/idatabase_builder.hpp>
#include <database/project_info.hpp>
#include <database/idatabase.hpp>

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


ProjectInfo ProjectManager::new_prj(const QString& prjName, const Database::IPlugin* prjPlugin)
{
    QDir storagePath(getPrjStorage());
    const QString prjId = getUniqueId();
    storagePath.mkdir(prjId);
    storagePath.cd(prjId);
    
    const QString prjDir = storagePath.absolutePath();
    const QString prjPath = storagePath.absoluteFilePath("broom.bpj");

    //prepare database
    Database::ProjectInfo dbPrjInfo = prjPlugin->initPrjDir(prjDir);

    //prepare project file
    QSettings prjFile(prjPath, QSettings::IniFormat);

    prjFile.beginGroup("Database");
    prjFile.setValue("backend", dbPrjInfo.backendName);
    prjFile.setValue("location", dbPrjInfo.databaseLocation);
    prjFile.endGroup();

    prjFile.beginGroup("Project");
    prjFile.setValue("name", prjName);
    prjFile.endGroup();

    const ProjectInfo prjInfo(prjName, prjId);

    return prjInfo;
}


std::deque<ProjectInfo> ProjectManager::listProjects()
{
    std::deque<ProjectInfo> result;
    
    QString path = getPrjStorage();
    
    if (path.isEmpty() == false)
    {
        const QDir basePath(path);
        const QStringList ids = basePath.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

        for (const QString& id: ids)
        {
            ProjectInfo info = get(id);

            result.push_back(info);
        }
    }
    
    return result;
}


std::unique_ptr<IProject> ProjectManager::open(const ProjectInfo& prjInfo, Database::IBuilder::OpenResult openResult)
{
    QDir storagePath(getPrjStorage());
    storagePath.cd(prjInfo.getId());

    const QString prjDir = storagePath.absolutePath();
    const QString prjPath = storagePath.absoluteFilePath("broom.bpj");

    QSettings prjFile(prjPath, QSettings::IniFormat);

    prjFile.beginGroup("Database");
    QString backend  = prjFile.value("backend").toString();
    QString location = prjFile.value("location").toString();
    prjFile.endGroup();

    Project* result = new Project();
    result->setPrjPath(prjPath);
    result->setDBBackend(backend);
    result->setDBLocation(location);
    result->setName(prjInfo.getName());

    Database::ProjectInfo dbPrjInfo(location, backend, prjDir);
    auto db = m_dbBuilder->get(dbPrjInfo, openResult);

    result->setDatabase(std::move(db));

    return std::unique_ptr<IProject>(result);
}


bool ProjectManager::save(const IProject* project)
{
    QSettings prjFile(project->getPrjPath(), QSettings::IniFormat);

    prjFile.beginGroup("Database");
    prjFile.setValue("backend",  project->getDBBackend());
    prjFile.setValue("location", project->getDBLocation());
    prjFile.endGroup();

    prjFile.beginGroup("Project");
    prjFile.setValue("name",  project->getName());
    prjFile.endGroup();

    return true;
}


bool ProjectManager::remove(const ProjectInfo& name)
{
    QDir storagePath(getPrjStorage());

    bool status = storagePath.exists(name.getId());

    if (status)
    {
        storagePath.cd(name.getId());
        status = storagePath.removeRecursively();
    }

    return status;
}


QString ProjectManager::getPrjStorage() const
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


ProjectInfo ProjectManager::get(const QString& id) const
{
    const QString prjs = getPrjStorage();
    QDir storageDir(prjs);

    QString name;

    if (storageDir.exists(id))
    {
        QDir prjDir(storageDir);
        prjDir.cd(id);
        const QString prjPath = prjDir.absoluteFilePath("broom.bpj");

        QSettings prjFile(prjPath, QSettings::IniFormat);

        prjFile.beginGroup("Project");
        name = prjFile.value("name").toString();
        prjFile.endGroup();
    }

    return ProjectInfo(name, id);
}


QString ProjectManager::getUniqueId() const
{
    const QString prjs = getPrjStorage();
    QDir storageDir(prjs);

    QString result;

    do
    {
        result.clear();

        for (int i = 0; i < 8; i++)
            result.append(rand() % 8 + '0');
    } while (storageDir.exists(result) == true);

    return result;
}
