/*
 * Database thread.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "database_thread.hpp"

#include <thread>
#include <memory>

#include <palgorithm/ts_queue.hpp>

namespace
{
    struct IThreadVisitor;

    struct IThreadTask
    {
        virtual ~IThreadTask() {}
        virtual void visitMe(IThreadVisitor *) = 0;
    };

    struct IThreadVisitor
    {
        virtual ~IThreadVisitor() {}
        virtual void visit(IThreadTask *) = 0;
    };

    struct StoreTask: IThreadTask
    {
        StoreTask(const PhotoInfo::Ptr& photo, Database::IDatabaseClient* client): m_photoInfo(photo), m_client(client) {}
        virtual ~StoreTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        PhotoInfo::Ptr m_photoInfo;
        Database::IDatabaseClient* m_client;
    };


    struct GetAllPhotosTask: IThreadTask
    {
        virtual ~GetAllPhotosTask() {}
        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }
    };


    struct GetPhotoTask: IThreadTask
    {
        virtual ~GetPhotoTask() {}
        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }
    };

    struct GetPhotosTask: IThreadTask
    {
        virtual ~GetPhotosTask() {}
        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }
    };

    struct ListTagsTask: IThreadTask
    {
        virtual ~ListTagsTask() {}
        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }
    };

    struct ListTagValuesTask: IThreadTask
    {
        virtual ~ListTagValuesTask() {}
        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }
    };


    struct Executor: IThreadVisitor
    {
        Executor(const std::shared_ptr<Database::IBackend>& backend): m_backend(backend), m_tasks(1024) {}

        virtual ~Executor() {}
        virtual void visit(IThreadTask*)
        {
        }

        void begin()
        {
            for(;;)
            {
                boost::optional< std::shared_ptr<IThreadTask> > task = m_tasks.pop_front();

                if (task)
                    (*task)->visitMe(this);
                else
                    break;
            }
        }

        std::shared_ptr<Database::IBackend> m_backend;
        TS_Queue< std::shared_ptr<IThreadTask> > m_tasks;
    };

}


namespace Database
{

    struct DatabaseThread::Impl
    {
        Impl(const std::shared_ptr<IBackend>& backend): m_executor(backend), m_thread(beginThread, &m_executor)
        {

        }


        void addTask(IThreadTask* task)
        {
            m_executor.m_tasks.push_back(std::shared_ptr<IThreadTask>(task));
        }

        static void beginThread(Executor* executor)
        {
            executor->begin();
        }

        Executor m_executor;
        std::thread m_thread;
    };

    DatabaseThread::DatabaseThread(std::unique_ptr<IBackend>&& backend): m_impl(new Impl(std::move(backend)))
    {

    }


    DatabaseThread::~DatabaseThread()
    {

    }


    void DatabaseThread::closeConnections()
    {
        //m_impl->m_backend->closeConnections();

        assert("!not implemented");
    }


    bool DatabaseThread::init(const char* name)
    {
        //const bool status = m_impl->m_backend->init(name);
        //return status;


    }


    Task DatabaseThread::store(const PhotoInfo::Ptr& photo, IDatabaseClient* client)
    {
        //const bool status = m_impl->m_backend->store(photo);

        StoreTask* task = new StoreTask(photo, client);
        m_impl->addTask(task);
    }


    Task DatabaseThread::getAllPhotos(IDatabaseClient* client)
    {
        //auto result = m_impl->m_backend->getAllPhotos();
        //client->got_getAllPhotos(result);

        GetAllPhotosTask* task = new GetAllPhotosTask;
        m_impl->addTask(task);
    }


    Task DatabaseThread::getPhoto(const PhotoInfo::Id& id, IDatabaseClient* client)
    {
        //auto result = m_impl->m_backend->getPhoto(id);
        //client->got_getPhoto(result);

        GetPhotoTask* task = new GetPhotoTask;
        m_impl->addTask(task);
    }


    Task DatabaseThread::getPhotos(const std::deque<IFilter::Ptr>& filter, IDatabaseClient* client)
    {
        //auto result = m_impl->m_backend->getPhotos(filter);
        //client->got_getPhotos(result);

        GetPhotosTask* task = new GetPhotosTask;
        m_impl->addTask(task);
    }


    Task DatabaseThread::listTags(IDatabaseClient* client)
    {
        //auto result = m_impl->m_backend->listTags();
        //client->got_listTags(result);

        ListTagsTask* task = new ListTagsTask;
        m_impl->addTask(task);
    }


    Task DatabaseThread::listTagValues(const TagNameInfo& info, IDatabaseClient* client)
    {
        //auto result = m_impl->m_backend->listTagValues(info);
        //client->got_listTagValues(result);

        ListTagValuesTask* task = new ListTagValuesTask;
        m_impl->addTask(task);
    }


    Task DatabaseThread::listTagValues(const TagNameInfo& info, const std::deque<IFilter::Ptr>& filter, IDatabaseClient* client)
    {
        //auto result = m_impl->m_backend->listTagValues(info, filter);
        //client->got_listTagValues(result);

        ListTagValuesTask* task = new ListTagValuesTask;
        m_impl->addTask(task);
    }

}
