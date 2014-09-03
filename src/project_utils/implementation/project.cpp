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

Project::Project(): m_backend(), m_location(), m_prjPath(), m_database(nullptr)
{

}


Project::~Project()
{

}


void Project::setPrjPath(const QString& prjPath)
{
    m_prjPath = prjPath;
}


void Project::setDBBackend(const QString& backend)
{
    m_backend = backend;
}


void Project::setDBLocation(const QString& location)
{
    m_location = location;
}


void Project::setDatabase(Database::IDatabase* database)
{
    m_database = database;
}


QString Project::getDBBackend() const
{
    return m_backend;
}


QString Project::getDBLocation() const
{
    return m_location;
}


QString Project::getPrjPath() const
{
    return m_prjPath;
}


Database::IDatabase* Project::getDatabase() const
{
    return m_database;
}

