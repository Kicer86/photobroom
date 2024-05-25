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

#include <core/utils.hpp>

#include "photo_data.hpp"


namespace Photo
{
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


    DataDelta& DataDelta::operator=(const DataDelta& other)
    {
        m_id = other.m_id;
        m_data = other.m_data;

        return *this;
    }


    void DataDelta::clear(Field field)
    {
        // TODO: improve it
        for_each<Field>([&](auto enum_v)
        {
            constexpr auto enum_value = decltype(enum_v)::value;

            if (enum_value == field)
                m_data[field] = typename DeltaTypes<enum_value>::Storage{};
        });
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


    const Id& DataDelta::getId() const
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
                m_data.insert_or_assign(otherData.first, otherData.second);
        }

        return *this;
    }

    DataDelta DataDelta::operator-(Photo::Field field) const
    {
        DataDelta copy(*this);
        copy.m_data.erase(field);

        return copy;
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
