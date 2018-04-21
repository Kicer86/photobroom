/*
 * struct with information about people.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "person_data.hpp"


PersonData::PersonData()
{

}


PersonData::PersonData(const Person::Id& id, const QString& name, const QString& path):
    m_id(id),
    m_name(name),
    m_path(path)
{

}


PersonData::PersonData(const PersonData& other):
    m_id(other.m_id),
    m_name(other.m_name),
    m_path(other.m_path)
{

}


const Person::Id& PersonData::id() const
{
    return m_id;
}

const QString& PersonData::name() const
{
    return m_name;
}


const QString& PersonData::path() const
{
    return m_path;
}
