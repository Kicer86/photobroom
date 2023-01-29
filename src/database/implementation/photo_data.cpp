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

    Data::Data(const Photo::Id& i)
        : id(i)
    {

    }

    Data& Data::apply(const DataDelta& delta)
    {
        id = delta.getId();

        if (delta.has(Photo::Field::Tags))
            tags = delta.get<Photo::Field::Tags>();

        if (delta.has(Photo::Field::Geometry))
            geometry = delta.get<Photo::Field::Geometry>();

        if (delta.has(Photo::Field::Flags))
            flags = delta.get<Photo::Field::Flags>();

        if (delta.has(Photo::Field::GroupInfo))
            groupInfo = delta.get<Photo::Field::GroupInfo>();

        if (delta.has(Photo::Field::Path))
            path = delta.get<Photo::Field::Path>();

        if (delta.has(Photo::Field::PHash))
            phash = delta.get<Photo::Field::PHash>();

        return *this;
    }


    DataDelta::DataDelta(const DataDelta& other)
    {
        m_id = other.m_id;
        m_data = other.m_data;
    }


    DataDelta::DataDelta(DataDelta&& other) noexcept
    {
        m_id = other.m_id;
        m_data = std::move(other.m_data);
    }


    DataDelta::DataDelta(const Data& oldData, const Data& newData)
    {
        assert(oldData.id == newData.id);

        setId(oldData.id);

        if (oldData.flags != newData.flags)
            insert<Photo::Field::Flags>(newData.flags);

        if (oldData.geometry != newData.geometry)
            insert<Photo::Field::Geometry>(newData.geometry);

        if (oldData.groupInfo != newData.groupInfo)
            insert<Photo::Field::GroupInfo>(newData.groupInfo);

        if (oldData.path != newData.path)
            insert<Photo::Field::Path>(newData.path);

        if (oldData.tags != newData.tags)
            insert<Photo::Field::Tags>(newData.tags);

        if (oldData.phash != newData.phash)
            insert<Photo::Field::PHash>(newData.phash);
    }


    DataDelta::DataDelta(const Photo::Data& data)
        : m_id(data.id)
    {
        this->operator=(data);
    }

    DataDelta& DataDelta::operator=(const DataDelta& other)
    {
        m_id = other.m_id;
        m_data = other.m_data;

        return *this;
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


    const Id & DataDelta::getId() const
    {
        return m_id;
    }


    bool DataDelta::operator<(const DataDelta& other) const
    {
        return m_id < other.m_id;
    }


    bool DataDelta::operator==(const DataDelta& other) const
    {
        return std::tie(m_id, m_data) == std::tie(other.m_id, other.m_data);
    }


    DataDelta& DataDelta::operator|=(const DataDelta& other)
    {
        assert(m_id.valid() == false || m_id == other.m_id);

        m_id = other.m_id;

        for(const auto& otherData: other.m_data)
        {
            if (otherData.first == Field::Flags && has(Field::Flags))
            {
                auto& flags = std::get<DeltaTypes<Field::Flags>::Storage>(m_data[Photo::Field::Flags]);
                const auto& otherFlags = std::get<DeltaTypes<Field::Flags>::Storage>(otherData.second);

                flags.insert(otherFlags.begin(), otherFlags.end());

            }
            else
                m_data.insert(otherData);
        }

        return *this;
    }


    DataDelta& DataDelta::operator=(const Data& data)
    {
        insert<Photo::Field::Tags>(data.tags);
        insert<Photo::Field::Geometry>(data.geometry);
        insert<Photo::Field::Flags>(data.flags);
        insert<Photo::Field::GroupInfo>(data.groupInfo);
        insert<Photo::Field::Path>(data.path);

        return *this;
    }


    const DataDelta::Storage& DataDelta::get(Photo::Field field) const
    {
        assert(has(field));
        auto it = m_data.find(field);

        return it->second;
    }


    DataDelta::Storage& DataDelta::get(Photo::Field field)
    {
        assert(has(field));
        auto it = m_data.find(field);

        return it->second;
    }
}
