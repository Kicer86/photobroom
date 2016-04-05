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
    internalLocation = QString("%1/%2_files").arg(baseDir).arg(name);
}


ProjectInfo::ProjectInfo(): path(), baseDir(), name()
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


///////////////////////////////////////////////////////////////////////////////


Project::Project(std::unique_ptr<Database::IDatabase>&& db, const ProjectInfo& prjInfo): m_database(std::move(db)), m_prjInfo(prjInfo)
{

}


Project::~Project()
{

}


Database::IDatabase* Project::getDatabase() const
{
    return m_database.get();
}


const ProjectInfo& Project::getProjectInfo() const
{
    return m_prjInfo;
}
