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

#include "photo_iterator.hpp"
#include "query_list.hpp"
#include "filter.hpp"

struct IConfiguration;

namespace Database
{

    struct IPhotoInfoManager;
    struct IPhotoInfoCreator;

    //Low level database interface.
    //To be used by particular database backend
    struct IBackend
    {
        virtual ~IBackend() {}

        virtual void setPhotoInfoManager(Database::IPhotoInfoManager *) = 0;
        virtual void setPhotoInfoCreator(Database::IPhotoInfoCreator *) = 0;

        virtual bool transactionsReady() = 0;       //transacions are ready after init()
        virtual bool beginTransaction() = 0;
        virtual bool endTransaction() = 0;

        //add photo to database
        virtual IPhotoInfo::Ptr addPath(const QString &) = 0;

        //update data
        virtual bool update(const IPhotoInfo::Ptr &) = 0;

        //read data
        virtual std::vector<TagNameInfo> listTags() = 0;                                  //list all stored tag names
        virtual std::set<TagValueInfo> listTagValues(const TagNameInfo &) = 0;            //list all values of provided tag
        virtual std::deque<TagValueInfo> listTagValues(const TagNameInfo &, const std::deque<IFilter::Ptr> &) = 0; //list all values for provided tag used on photos matching provided filter
        virtual QueryList getAllPhotos() = 0;                                             //list all photos
        virtual QueryList getPhotos(const std::deque<IFilter::Ptr> &) = 0;                //list all photos matching filter
        virtual IPhotoInfo::Ptr getPhoto(const IPhotoInfo::Id &) = 0;                     //get particular photo

        //init backend - connect to database or create new one
        virtual bool init(const QString &) = 0;
        virtual void set(IConfiguration *) = 0;

        //close database connection
        virtual void closeConnections() = 0;
    };

    struct IDatabaseClient;

    //Task structure.
    //Used by database to identify task and associated client.
    struct Task
    {
        typedef int Id;

        Task(IDatabaseClient* c, int i): id(i), status(false), client(c) {}

        void setStatus(bool s) { status = s; }

        IDatabaseClient* getClient() const { return client; }
        Id getId() const { return id; }
        bool getStatus() const { return status; }

        bool operator==(const Task& other) const { return id == other.id; }

        private:
            Id id;
            bool status;
            IDatabaseClient* client;
    };


    //Database client.
    //Prepared for database clients.
    //Set of functions called when particalar task is finished
    struct IDatabaseClient
    {
        virtual ~IDatabaseClient() {}

        virtual void got_storeStatus(const Task &) = 0;

        virtual void got_listTags(const Task &, const std::vector<TagNameInfo> &) = 0;
        virtual void got_listTagValues(const Task &, const std::deque<TagValueInfo> &) = 0;
        virtual void got_getAllPhotos(const Task &, const QueryList &) = 0;
        virtual void got_getPhotos(const Task &, const QueryList &) = 0;
        virtual void got_getPhoto(const Task &, const IPhotoInfo::Ptr &) = 0;
    };

    //set of signals emitted when database changes or when tasks are being executed
    class ADatabaseSignals: public QObject
    {
            Q_OBJECT

        signals:
            void photoAdded(const IPhotoInfo::Ptr &);     //emited when new photo added
            void photoModified(const IPhotoInfo::Ptr &);  //emited when photo updated

            void beforeTaskExecution(const Task::Id &);   //emited before task execution
            void afterTaskExecution(const Task::Id &);    //emited after task execution
    };

    //Database interface.
    //A bridge between clients and backend.
    struct IDatabase
    {
        virtual ~IDatabase() {}

        virtual Task prepareTask(IDatabaseClient *) = 0;
        virtual ADatabaseSignals* notifier() = 0;

        //store data
        virtual void addPath(const Task &, const QString &) = 0;
        virtual void update(const Task &, const IPhotoInfo::Ptr &) = 0;

        //read data
        virtual void listTags(const Task &) = 0;                                     //list all stored tag names
        virtual void listTagValues(const Task &, const TagNameInfo &) = 0;           //list all values of provided tag
        virtual void listTagValues(const Task &, const TagNameInfo &, const std::deque<IFilter::Ptr> &) = 0; //list all values for provided tag used on photos matching provided filter
        virtual void getAllPhotos(const Task &) = 0;                                 //list all photos
        virtual void getPhotos(const Task &, const std::deque<IFilter::Ptr> &) = 0;  //list all photos matching filter
        virtual void getPhoto(const Task &, const IPhotoInfo::Id &) = 0;             //get particulat photo

        //init backend - connect to database or create new one
        virtual bool init(const Database::Task &, const QString &) = 0;

        //close database connection
        virtual void closeConnections() = 0;
    };

}

#endif
