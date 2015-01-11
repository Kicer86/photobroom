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

#include <QObject>

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
    struct IDatabaseClient;
    struct ProjectInfo;

    //set of signals emitted when database changes or when tasks are being executed
    class ADatabaseSignals: public QObject
    {
            Q_OBJECT

        signals:
            void photoAdded(const IPhotoInfo::Ptr &);     //emited when new photo was added to database
            void photoModified(const IPhotoInfo::Ptr &);  //emited when photo updated
    };

    struct IStorePhotoTask
    {
        virtual ~IStorePhotoTask() {}

        virtual void got(bool) = 0;
    };

    struct IStoreTagTask
    {
        virtual ~IStoreTagTask() {}

        virtual void got(bool) = 0;
    };

    struct IListTagsTask
    {
        virtual ~IListTagsTask() {}

        virtual void got(const std::deque<TagNameInfo> &) = 0;
    };

    struct IListTagValuesTask
    {
        virtual ~IListTagValuesTask() {}

        virtual void got(const TagValue::List &) = 0;
    };

    struct IGetPhotosTask
    {
        virtual ~IGetPhotosTask() {}

        virtual void got(const IPhotoInfo::List &) = 0;
    };

    struct IGetPhotoTask
    {
        virtual ~IGetPhotoTask() {}

        virtual void got(const IPhotoInfo::Ptr &) = 0;
    };

    struct IGetPhotosCount
    {
        virtual ~IGetPhotosCount() {}

        virtual void got(int) = 0;
    };

    struct IInitTask
    {
        virtual ~IInitTask() {}

        virtual void got(bool) = 0;
    };

    //Database interface.
    //A bridge between clients and backend.
    struct IDatabase
    {
        virtual ~IDatabase() {}

        virtual ADatabaseSignals* notifier() = 0;

        //store data
        virtual void exec(std::unique_ptr<IStorePhotoTask> &&, const QString &) = 0;
        virtual void exec(std::unique_ptr<IStorePhotoTask> &&, const IPhotoInfo::Ptr &) = 0;
        virtual void exec(std::unique_ptr<IStoreTagTask> &&, const TagNameInfo &) = 0;

        //read data
        virtual void exec(std::unique_ptr<IListTagsTask> &&) = 0;                                     //list all stored tag names
        virtual void exec(std::unique_ptr<IListTagValuesTask> &&, const TagNameInfo &) = 0;           //list all values of provided tag
        virtual void exec(std::unique_ptr<IListTagValuesTask> &&, const TagNameInfo &, const std::deque<IFilter::Ptr> &) = 0; //list all values for provided tag used on photos matching provided filter
        virtual void exec(std::unique_ptr<IGetPhotosTask> &&) = 0;                                        //list all photos
        virtual void exec(std::unique_ptr<IGetPhotosTask> &&, const std::deque<IFilter::Ptr> &) = 0;      //list all photos matching filter
        virtual void exec(std::unique_ptr<IGetPhotoTask> &&, const IPhotoInfo::Id &) = 0;                 //get particulat photo
        virtual void exec(std::unique_ptr<IGetPhotosCount> &&, const std::deque<IFilter::Ptr> &) = 0;     //is there any photo matching filters?

        //init backend - connect to database or create new one
        virtual bool exec(std::unique_ptr<IInitTask> &&, const Database::ProjectInfo &) = 0;

        //close database connection
        virtual void closeConnections() = 0;
    };

}

#endif
