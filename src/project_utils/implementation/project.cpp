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

#include <QString>

#include <database/idatabase_builder.hpp>


Project::Project(std::unique_ptr<Database::IDBPack>&& db, const ProjectInfo& prjInfo): m_database(std::move(db)), m_prjInfo(prjInfo)
{

}


Project::~Project()
{

}


Database::IDatabase* Project::getDatabase() const
{
    return m_database->get();
}


const ProjectInfo& Project::getProjectInfo() const
{
    return m_prjInfo;
}
