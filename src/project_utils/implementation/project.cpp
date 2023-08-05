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

#include "project.hpp"

#include <QDir>
#include <QFileInfo>
#include <QString>


ProjectInfo::ProjectInfo(const QString& _path): ProjectInfo()
{
    const QFileInfo fi(_path);
    path = fi.absoluteFilePath();
    baseDir = fi.absolutePath();
    name = fi.baseName();
    internalLocation = QString("%1/photo_broom_%2_files").arg(baseDir).arg(name);
}


ProjectInfo::ProjectInfo(): path(), baseDir(), name(), internalLocation()
{

}


bool ProjectInfo::isValid() const
{
    return path.isEmpty() == false;
}


const QString& ProjectInfo::getPath() const
{
    return path;
}


const QString& ProjectInfo::getBaseDir() const
{
    return baseDir;
}


const QString& ProjectInfo::getName() const
{
    return name;
}


const QString& ProjectInfo::getInternalLocation() const
{
    return internalLocation;
}


QString ProjectInfo::getInternalLocation(InternalData dataType) const
{
    QString subdir;

    switch(dataType)
    {
        case Database:          subdir = "db";          break;
        case PrivateMultimedia: subdir = "multimedia";  break;
    }

    const QString result = QString("%1/%2").arg(internalLocation).arg(subdir);

    return result;
}


///////////////////////////////////////////////////////////////////////////////

const char* Project::ignoreFileName = ".photo_broom_ignore";


Project::Project(Database::IBuilder& dbBuilder, const ProjectInfo& prjInfo):
    m_prjInfo(prjInfo),
    m_dbBuilder(dbBuilder),
    m_lock(prjInfo.getPath() + ".lock")
{
    // create internal directories
    QDir().mkpath(m_prjInfo.getInternalLocation(ProjectInfo::PrivateMultimedia));
}


Project::~Project()
{
    m_lock.unlock();
}


bool Project::lockProject()
{
    return m_lock.tryLock();
}


void Project::openDatabase(const Database::ProjectInfo& prjInfo, const Database::IDatabaseRoot::OpenCallback& openCallback)
{
    m_database = m_dbBuilder.get(prjInfo);
    m_database->init(prjInfo, openCallback);
}


void Project::closeDatabase()
{
    m_database->close();
}


Database::IDatabase& Project::getDatabase() const
{
    return *m_database.get();
}


const ProjectInfo& Project::getProjectInfo() const
{
    return m_prjInfo;
}


QString Project::makePathRelative(const QString& path) const
{
    const QString baseDir = m_prjInfo.getBaseDir();
    const auto l = baseDir.size();
    const bool subdir = path.left(l) == baseDir;

    assert(subdir);

    const QString result = "prj:" + path.mid(l);
    return result;
}


QString Project::makePathAbsolute(const QString& relative) const
{
    const QFileInfo info(relative);
    const QString absolute = info.absoluteFilePath();

    return absolute;
}
