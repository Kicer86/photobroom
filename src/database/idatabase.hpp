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
#include "database_status.hpp"
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

    struct AStorePhotoTask
    {
        virtual ~AStorePhotoTask() {}

        virtual void got(bool) = 0;
    };

    struct AStoreTagTask
    {
        virtual ~AStoreTagTask() {}

        virtual void got(bool) = 0;
    };

    struct AListTagsTask
    {
        virtual ~AListTagsTask() {}

        virtual void got(const std::deque<TagNameInfo> &) = 0;
    };

    struct AListTagValuesTask
    {
        virtual ~AListTagValuesTask() {}

        virtual void got(const std::deque<QVariant> &) = 0;
    };

    struct AGetPhotosTask
    {
        virtual ~AGetPhotosTask() {}

        virtual void got(const IPhotoInfo::List &) = 0;
    };

    struct AGetPhotoTask
    {
        virtual ~AGetPhotoTask() {}

        virtual void got(const IPhotoInfo::Ptr &) = 0;
    };

    struct AGetPhotosCount
    {
        virtual ~AGetPhotosCount() {}

        virtual void got(int) = 0;
    };

    struct AInitTask
    {
        virtual ~AInitTask() {}

        virtual void got(const Database::BackendStatus &) = 0;
    };

    //Database interface.
    //A bridge between clients and backend.
    struct IDatabase
    {
        virtual ~IDatabase() {}

        virtual ADatabaseSignals* notifier() = 0;

        //store data
        virtual void exec(std::unique_ptr<AStorePhotoTask> &&, const QString &) = 0;
        virtual void exec(std::unique_ptr<AStorePhotoTask> &&, const IPhotoInfo::Ptr &) = 0;
        virtual void exec(std::unique_ptr<AStoreTagTask> &&, const TagNameInfo &) = 0;

        //read data
        virtual void exec(std::unique_ptr<AListTagsTask> &&) = 0;                                         //list all stored tag names
        virtual void exec(std::unique_ptr<AListTagValuesTask> &&, const TagNameInfo &) = 0;               //list all values of provided tag
        virtual void exec(std::unique_ptr<AListTagValuesTask> &&, const TagNameInfo &, const std::deque<IFilter::Ptr> &) = 0; //list all values for provided tag used on photos matching provided filter
        virtual void exec(std::unique_ptr<AGetPhotosTask> &&) = 0;                                        //list all photos
        virtual void exec(std::unique_ptr<AGetPhotosTask> &&, const std::deque<IFilter::Ptr> &) = 0;      //list all photos matching filter
        virtual void exec(std::unique_ptr<AGetPhotoTask> &&, const IPhotoInfo::Id &) = 0;                 //get particulat photo
        virtual void exec(std::unique_ptr<AGetPhotosCount> &&, const std::deque<IFilter::Ptr> &) = 0;     //is there any photo matching filters?

        //init backend - connect to database or create new one
        virtual void exec(std::unique_ptr<AInitTask> &&, const Database::ProjectInfo &) = 0;

        //close database connection
        virtual void closeConnections() = 0;
    };

}

#endif
