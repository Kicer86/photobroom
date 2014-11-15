/*
 *    interface for backends
 *    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef IBACKEND_HPP
#define IBACKEND_HPP

#include <string>
#include <set>
#include <deque>

#include <core/tag.hpp>
#include <database/iphoto_info.hpp>

//#include "photo_iterator.hpp"
//#include "query_list.hpp"
#include "filter.hpp"

struct ILogger;
struct IConfiguration;

namespace Database
{

    struct IPhotoInfoCache;
    struct IPhotoInfoCreator;
    struct ProjectInfo;

    //Low level database interface.
    //To be used by particular database backend
    struct IBackend
    {
        virtual ~IBackend() {}

        virtual void setPhotoInfoManager(Database::IPhotoInfoCache *) = 0;
        virtual void setPhotoInfoCreator(Database::IPhotoInfoCreator *) = 0;

        virtual bool transactionsReady() = 0;       //transacions are ready after init()
        virtual bool beginTransaction() = 0;
        virtual bool endTransaction() = 0;

        //add photo to database
        virtual IPhotoInfo::Ptr addPath(const QString &) = 0;

        //update data
        virtual bool update(const IPhotoInfo::Ptr &) = 0;

        //read data
        virtual std::deque<TagNameInfo> listTags() = 0;                                   //list all stored tag names
        virtual TagValue listTagValues(const TagNameInfo &) = 0;                          //list all values of provided tag
        virtual TagValue listTagValues(const TagNameInfo &, const std::deque<IFilter::Ptr> &) = 0; //list all values for provided tag used on photos matching provided filter
        virtual IPhotoInfo::List getAllPhotos() = 0;                                      //list all photos
        virtual IPhotoInfo::Ptr getPhoto(const IPhotoInfo::Id &) = 0;                     //get particular photo
        virtual IPhotoInfo::List getPhotos(const std::deque<IFilter::Ptr> &) = 0;         //find all photos matching filter

        //init backend - connect to database or create new one
        virtual bool init(const ProjectInfo &) = 0;
        virtual void set(IConfiguration *) = 0;
        virtual void set(ILogger *) = 0;

        //close database connection
        virtual void closeConnections() = 0;
    };
}

#endif

