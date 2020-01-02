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

#include <cassert>

#include "photo_data.hpp"

namespace Photo
{

    int Data::getFlag(const Photo::FlagsE& flag) const
    {
        auto it = flags.find(flag);

        const int result = it == flags.end()? 0: it->second;

        return result;
    }


    Data::Data():
        id(),
        sha256Sum(),
        tags(),
        flags(),
        path(),
        geometry(),
        groupInfo()
    {

    }


    void DataDelta::setId(const Photo::Id& id)
    {
        assert(m_id.valid() == false);      // do we expect id to be set more than once?
        m_id = id;
    }


    void DataDelta::clear()
    {
        m_data.clear();
        m_id = Photo::Id();
    }


    bool DataDelta::has(Photo::Field field) const
    {
        auto it = m_data.find(field);

        return it != m_data.end();
    }


    const std::any& DataDelta::get(Photo::Field field) const
    {
        assert(has(field));
        auto it = m_data.find(field);

        return it->second;
    }


    const Id & DataDelta::getId() const
    {
        return m_id;
    }


    bool DataDelta::operator<(const DataDelta& other) const
    {
        return m_id < other.m_id;
    }

}
