/*
    Database entries manipulator
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


#include "entry.hpp"

#include "core/hash_functions.hpp"
#include "ifs.hpp"


namespace Database
{

    Entry::Entry(): m_d(new Entry::Data)
    {

    }


    Entry::Entry(Entry&& other): m_d(std::move(other.m_d))
    {
        this->operator=(std::move(other));
    }


    Entry::Entry(const APhotoInfo::Ptr& ptr):
        m_d(new Entry::Data(ptr))
    {

    }


    Entry::~Entry()
    {
    }


    Entry& Entry::operator=(Entry && other)
    {
        m_d = std::move(other.m_d);
        other.m_d = nullptr;

        return *this;
    }


    bool Entry::operator==(const Entry&) const
    {
        return false;
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////


    Entry::Data::Data(const APhotoInfo::Ptr& photoInfo):
        m_photoInfo(photoInfo)
    {

    }

}
