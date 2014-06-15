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
#include <set>
#include <memory>
#include <deque>

#include <core/photo_info.hpp>
#include <core/tag.hpp>

#include "photo_iterator.hpp"
#include "query_list.hpp"
#include "filter.hpp"

namespace Database
{

    struct IBackend
    {
        virtual ~IBackend() {}

        //store data
        virtual bool store(const PhotoInfo::Ptr &) = 0;

        //read data
        virtual std::vector<TagNameInfo> listTags() = 0;                                  //list all stored tag names
        virtual std::set<TagValueInfo> listTagValues(const TagNameInfo &) = 0;            //list all values of provided tag
        virtual std::deque<TagValueInfo> listTagValues(const TagNameInfo &, const std::deque<IFilter::Ptr> &) = 0; //list all values for provided tag used on photos matching provided filter
        virtual QueryList getAllPhotos() = 0;                                             //list all photos
        virtual QueryList getPhotos(const std::deque<IFilter::Ptr> &) = 0;                //list all photos matching filter
        virtual PhotoInfo::Ptr getPhoto(const PhotoInfo::Id &) = 0;                       //get particulat photo

        //init backend - connect to database or create new one
        virtual bool init(const char *) = 0;

        //close database connection
        virtual void closeConnections() = 0;
    };

    struct IFrontend            //TODO: deprecated?
    {
        virtual ~IFrontend() {}

        virtual void setBackend(IBackend *) = 0;

        virtual void close() = 0;
    };
}

#endif
