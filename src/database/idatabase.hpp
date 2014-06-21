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

    //direct database interface
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


    struct Task
    {
        typedef int Id;

        Id id;
        bool status;
    };


    struct IDatabaseClient
    {
        virtual void got_listTags(const Task &, const std::vector<TagNameInfo> &) = 0;
        virtual void got_listTagValues(const Task &, const std::set<TagValueInfo> &) = 0;
        virtual void got_listTagValues(const Task &, const std::deque<TagValueInfo> &) = 0;
        virtual void got_getAllPhotos(const Task &, const QueryList &) = 0;
        virtual void got_getPhotos(const Task &, const QueryList &) = 0;
        virtual void got_getPhoto(const Task &, const PhotoInfo::Ptr &) = 0;
    };


    //for final database clients
    struct IDatabase
    {
        virtual ~IDatabase() {}

        //store data
        virtual Task store(const PhotoInfo::Ptr &, IDatabaseClient *) = 0;

        //read data
        virtual Task listTags(IDatabaseClient *) = 0;                                     //list all stored tag names
        virtual Task listTagValues(const TagNameInfo &, IDatabaseClient *) = 0;           //list all values of provided tag
        virtual Task listTagValues(const TagNameInfo &, const std::deque<IFilter::Ptr> &, IDatabaseClient *) = 0; //list all values for provided tag used on photos matching provided filter
        virtual Task getAllPhotos(IDatabaseClient *) = 0;                                 //list all photos
        virtual Task getPhotos(const std::deque<IFilter::Ptr> &, IDatabaseClient *) = 0;  //list all photos matching filter
        virtual Task getPhoto(const PhotoInfo::Id &, IDatabaseClient *) = 0;              //get particulat photo

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
