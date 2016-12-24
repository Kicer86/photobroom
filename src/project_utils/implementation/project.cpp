/*
 * Project data
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

#include <QFileInfo>
#include <QString>

#include <database/idatabase_builder.hpp>


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


QString ProjectInfo::getInternalLocation(InternalData dataType) const
{
    QString subdir;

    switch(dataType)
    {
        case Database:          subdir = "db";    break;
        case PrivateMultimedia: subdir = "media"; break;
    }

    const QString result = QString("%1/%2").arg(internalLocation).arg(subdir);

    return result;
}


///////////////////////////////////////////////////////////////////////////////


Project::Project(std::unique_ptr<Database::IDatabase>&& db, const ProjectInfo& prjInfo):
    m_prjInfo(prjInfo),
    m_database(std::move(db)),
    m_lock(prjInfo.getPath() + ".lock")
{

}


Project::~Project()
{
    m_lock.unlock();
}


bool Project::lockProject()
{
    return m_lock.tryLock();
}


Database::IDatabase* Project::getDatabase() const
{
    return m_database.get();
}


const ProjectInfo& Project::getProjectInfo() const
{
    return m_prjInfo;
}


QString Project::makePathRelative(const QString& path) const
{
    const QString baseDir = m_prjInfo.getBaseDir();
    const int l = baseDir.size();

    const bool subdir = path.left(l) == baseDir;

    assert(subdir);

    const QString result = "prj:" + path.mid(l);
    return result;
}
