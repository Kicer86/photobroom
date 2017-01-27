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

#include "action.hpp"
#include "database_status.hpp"
#include "filter.hpp"
#include "group.hpp"
#include "iphoto_info.hpp"

#include "database_export.h"

struct ILogger;
struct IConfiguration;

namespace Database
{

    struct IPhotoInfoCache;
    struct IPhotoInfoCreator;
    struct IDatabaseClient;
    struct ProjectInfo;

    //set of signals emitted when database changes or when tasks are being executed
    class DATABASE_EXPORT ADatabaseSignals: public QObject
    {
            Q_OBJECT

        signals:
            void photosAdded(const std::deque<IPhotoInfo::Ptr> &);  // emited after new photos were added to database
            void photoModified(const IPhotoInfo::Ptr &);            // emited when photo updated
            void photosRemoved(const std::deque<Photo::Id> &);      // emited after photos removal
    };

    struct DATABASE_EXPORT AInitTask
    {
        virtual ~AInitTask() = default;

        virtual void got(const Database::BackendStatus &) = 0;
    };

    //Database interface.
    //A bridge between clients and backend.
    // TODO: all exec functions should be dropped and dedicated functions should be introduced
    struct DATABASE_EXPORT IDatabase
    {
        template <typename... Args>
        using Callback = std::function<void(Args...)>;

        virtual ~IDatabase() = default;

        virtual ADatabaseSignals* notifier() = 0;

        // store data
        virtual void update(const IPhotoInfo::Ptr &) = 0;
        virtual void store(const std::set< QString >&, const Callback<const std::vector<Photo::Id> &>& = Callback<const std::vector<Photo::Id> &>()) = 0;
        virtual void createGroup(const Photo::Id &, const Callback<Group::Id> &) = 0;

        // read data
        virtual void countPhotos(const std::deque<IFilter::Ptr> &, const Callback<int> &) = 0;                           // count photos matching filters
        virtual void getPhotos(const std::vector<Photo::Id> &, const Callback<std::deque<IPhotoInfo::Ptr>> &) = 0;       // get particular photos
        virtual void listTagNames( const Callback<const std::deque<TagNameInfo> &> & ) = 0;                              // list all stored tag names
        virtual void listTagValues( const TagNameInfo &, const Callback<const TagNameInfo &, const std::deque<TagValue> &> &) = 0;    // list all tag values
        virtual void listTagValues( const TagNameInfo &, const std::deque<IFilter::Ptr> &, const Callback<const TagNameInfo &, const std::deque<TagValue> &> &) = 0;  // list values of provided tag on photos matching filter
        virtual void listPhotos(const std::deque<IFilter::Ptr> &, const Callback<const IPhotoInfo::List &> &) = 0;       // list all photos matching filter

        // modify data
        virtual void perform(const std::deque<IFilter::Ptr> &, const std::deque<IAction::Ptr> &) = 0;     // perform actions on matching photos

        // drop data

        //init backend - connect to database or create new one
        [[deprecated]] virtual void exec(std::unique_ptr<AInitTask> &&, const Database::ProjectInfo &) = 0;

        //close database connection
        virtual void closeConnections() = 0;
    };

}

#endif
