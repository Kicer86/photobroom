
/*
* Basic data about photo
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

#include "photo_data.hpp"

namespace Database
{

    Id::Id(): m_value(-1), m_valid(false)
    {

    }


    Id::Id(Id::type v): m_value(v), m_valid(true)
    {

    }


    bool Id::operator!() const
    {
        return !m_valid;
    }


    Id::operator Id::type() const
    {
        return m_value;
    }


    bool Id::valid() const
    {
        return m_valid;
    }


    Id::type Id::value() const
    {
        return m_value;
    }


    /**************************************************************************/


    int PhotoData::getFlag(const Database::FlagsE& flag) const
    {
        auto it = flags.find(flag);

        const int result = it == flags.end()? 0: it->second;

        return result;
    }

}
