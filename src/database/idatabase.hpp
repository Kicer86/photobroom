/*
    interface for databases
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

#ifndef IDATABASE_HPP
#define IDATABASE_HPP

#include <string>
#include <map>
#include <memory>

#include "core/photo_info.hpp"

namespace Database
{

    class Entry;

    struct IBackend
    {
        virtual ~IBackend() {}

        virtual bool store(const Database::Entry&) = 0;
        virtual bool init() = 0;
    };

    struct IFrontend
    {
        virtual ~IFrontend() {}

        virtual bool addPhoto(const APhotoInfo::Ptr&) = 0;
        virtual void setBackend(const std::shared_ptr<IBackend>&) = 0;
    };
}

#endif
