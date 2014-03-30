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

#include <core/aphoto_info.hpp>
#include <core/tag.hpp>

#include "photo_iterator.hpp"

namespace Database
{

    struct SortInfo
    {
        QString tagName;
        bool ascending;
    };


    struct IBackend
    {
        virtual ~IBackend() {}

        //store data
        virtual bool store(const APhotoInfo::Ptr &) = 0;

        //read data
        virtual std::vector<TagNameInfo> listTags() = 0;
        virtual std::vector<TagValueInfo> listTagValues(const TagNameInfo &) = 0;
        virtual PhotoIterator getPhotos() = 0;
        virtual unsigned int getPhotosCount() = 0;

        //various
        virtual void setPhotosSorting(const std::vector<SortInfo> &) = 0;

        //init backend - connect to database or create new one
        virtual bool init() = 0;

        //close database connection
        virtual void closeConnections() = 0;
    };

    struct IFrontend            //TODO: deprecated?
    {
        virtual ~IFrontend() {}

        virtual bool addPhoto(const APhotoInfo::Ptr&) = 0;
        virtual void setBackend(const std::shared_ptr<IBackend>&) = 0;

        virtual void close() = 0;
    };
}

#endif
