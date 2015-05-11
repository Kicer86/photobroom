/*
 * Release version
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "version.hpp"

Version::Version(): m_major(0), m_minor(0), m_patch(0), m_valid(false)
{

}


Version::~Version()
{

}


bool Version::operator==(const Version& other) const
{
    return m_major == other.m_major &&
           m_minor == other.m_minor &&
           m_patch == other.m_patch;
}


bool Version::operator<(const Version& other) const
{
    bool status = false;

    if (m_major < other.m_major)
        status = true;
    else if (m_major == other.m_major)
    {
        if (m_minor < other.m_minor)
            status = true;
        else if (m_minor == other.m_minor)
            status = m_patch < other.m_patch;
    }

    return status;
}


bool Version::operator>(const Version& other) const
{
    bool status = false;

    if (m_major > other.m_major)
        status = true;
    else if (m_major == other.m_major)
    {
        if (m_minor > other.m_minor)
            status = true;
        else if (m_minor == other.m_minor)
            status = m_patch > other.m_patch;
    }

    return status;
}
