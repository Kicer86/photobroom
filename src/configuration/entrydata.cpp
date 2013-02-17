/*
    Configuration entry
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "entrydata.hpp"

#include <string>

struct EntryData::Data
{
    Data(): m_key(), m_value() {}

    Data(const Data &other): m_key(), m_value()
    {
        m_key = other.m_key;
        m_value = other.m_value;
    }

    Data(const std::string &key, const std::string &value): m_key(key), m_value(value)
    {

    }

    virtual ~Data() {}

    std::string m_key;
    std::string m_value;
};


EntryData::EntryData(): m_data(new Data)
{

}


EntryData::~EntryData()
{

}


bool EntryData::operator==(const EntryData& other) const
{
    const bool status = m_data->m_key == other.m_data->m_key;

    return status;
}
