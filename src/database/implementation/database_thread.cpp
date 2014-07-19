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
    struct InitTask;
    struct StoreTask;
    struct GetAllPhotosTask;
    struct GetPhotoTask;
    struct GetPhotosTask;
    struct ListTagsTask;
    struct ListTagValuesTask;

    struct IThreadTask
    {
        virtual ~IThreadTask() {}
        virtual void visitMe(IThreadVisitor *) = 0;
    };

    struct ThreadBaseTask: IThreadTask
    {
        ThreadBaseTask(const Database::Task& task): m_task(task) {}
        virtual ~ThreadBaseTask() {}

        Database::Task m_task;
    };

    struct IThreadVisitor
    {
        virtual ~IThreadVisitor() {}

        virtual void visit(InitTask *) = 0;
        virtual void visit(StoreTask *) = 0;
        virtual void visit(GetAllPhotosTask *) = 0;
        virtual void visit(GetPhotoTask *) = 0;
        virtual void visit(GetPhotosTask *) = 0;
        virtual void visit(ListTagsTask *) = 0;
        virtual void visit(ListTagValuesTask *) = 0;
    };

    struct InitTask: ThreadBaseTask
    {
        InitTask(const Database::Task& task, const std::string& name): ThreadBaseTask(task), m_name(name) {}
        virtual ~InitTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::string m_name;
    };

    struct StoreTask: ThreadBaseTask
    {
        StoreTask(const Database::Task& task, const PhotoInfo::Ptr& photo): ThreadBaseTask(task), m_photoInfo(photo) {}
        virtual ~StoreTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        PhotoInfo::Ptr m_photoInfo;
    };


    struct GetAllPhotosTask: ThreadBaseTask
    {
        GetAllPhotosTask(const Database::Task& task): ThreadBaseTask(task) {}
        virtual ~GetAllPhotosTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }
    };


    struct GetPhotoTask: ThreadBaseTask
    {
        GetPhotoTask(const Database::Task& task, const PhotoInfo::Id& id): ThreadBaseTask(task), m_id(id) {}
        virtual ~GetPhotoTask() {}
        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        PhotoInfo::Id m_id;
    };

    struct GetPhotosTask: ThreadBaseTask
    {
        GetPhotosTask(const Database::Task& task, const std::deque<Database::IFilter::Ptr>& filter): ThreadBaseTask(task), m_filter(filter) {}
        virtual ~GetPhotosTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::deque<Database::IFilter::Ptr> m_filter;
    };

    struct ListTagsTask: ThreadBaseTask
    {
        ListTagsTask(const Database::Task& task): ThreadBaseTask(task) {}
        virtual ~ListTagsTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }
    };

    struct ListTagValuesTask: ThreadBaseTask
    {
        ListTagValuesTask(const Database::Task& task, const TagNameInfo& info, const std::deque<Database::IFilter::Ptr>& filter): ThreadBaseTask(task), m_info(info), m_filter(filter) {}
        virtual ~ListTagValuesTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        TagNameInfo m_info;
        std::deque<Database::IFilter::Ptr> m_filter;
    };


    struct Executor: IThreadVisitor, Database::ADatabaseSignals
    {
        Executor(const std::shared_ptr<Database::IBackend>& backend): m_backend(backend), m_tasks(1024) {}

        virtual ~Executor() {}

        virtual void visit(InitTask* task)
        {
            m_backend->init(task->m_name.c_str());

            //TODO: result
        }

        virtual void visit(StoreTask* task)
        {
            const bool status = m_backend->store(task->m_photoInfo);
            task->m_task.setStatus(status);
            task->m_task.getClient()->got_storeStatus(task->m_task);

            emit photoModified(task->m_photoInfo->getID());
        }

        virtual void visit(GetAllPhotosTask* task)
        {
            auto result = m_backend->getAllPhotos();
            task->m_task.setStatus(true);
            task->m_task.getClient()->got_getAllPhotos(task->m_task, result);
        }

        virtual void visit(GetPhotoTask* task)
        {
            auto result = m_backend->getPhoto(task->m_id);
            task->m_task.setStatus(true);
            task->m_task.getClient()->got_getPhoto(task->m_task, result);
        }

        virtual void visit(GetPhotosTask* task)
        {
            auto result = m_backend->getPhotos(task->m_filter);
            task->m_task.setStatus(true);
            task->m_task.getClient()->got_getPhotos(task->m_task, result);
        }

        virtual void visit(ListTagsTask* task)
        {
            auto result = m_backend->listTags();
            task->m_task.setStatus(true);
            task->m_task.getClient()->got_listTags(task->m_task, result);
        }

        virtual void visit(ListTagValuesTask* task)
        {
            auto result = m_backend->listTagValues(task->m_info, task->m_filter);
            task->m_task.setStatus(true);
            task->m_task.getClient()->got_listTagValues(task->m_task, result);
        }


        void begin()
        {
            for(;;)
            {
                Optional< std::shared_ptr<IThreadTask> > task = m_tasks.pop_front();

                if (task)
                    (*task)->visitMe(this);
                else
                    break;
            }
        }

        std::shared_ptr<Database::IBackend> m_backend;
        TS_Queue<std::shared_ptr<IThreadTask>> m_tasks;
    };

}


namespace Database
{

    struct DatabaseThread::Impl
    {
        Impl(const std::shared_ptr<IBackend>& backend): m_lastId(0), m_executor(backend), m_thread(beginThread, &m_executor)
        {

        }

        //store task to be executed by thread
        void addTask(ThreadBaseTask* task)
        {
            m_executor.m_tasks.push_back(std::shared_ptr<IThreadTask>(task));
        }

        static void beginThread(Executor* executor)
        {
            executor->begin();
        }

        void stopExecutor()
        {
            m_executor.m_tasks.stop();

            assert(m_thread.joinable());
            m_thread.join();
        }

        int m_lastId;
        Executor m_executor;
        std::thread m_thread;
    };


    DatabaseThread::DatabaseThread(std::unique_ptr<IBackend>&& backend): m_impl(new Impl(std::move(backend)))
    {

    }


    DatabaseThread::~DatabaseThread()
    {
        //terminate thread
        m_impl->stopExecutor();
    }


    void DatabaseThread::closeConnections()
    {
        //m_impl->m_backend->closeConnections();

        assert("!not implemented");
    }


    bool DatabaseThread::init(const Task& db_task, const std::string& name)
    {
        InitTask* task = new InitTask(db_task, name);
        m_impl->addTask(task);

        //TODO: fix it
        return true;
    }


    Task DatabaseThread::prepareTask(IDatabaseClient* client)
    {
        Task task(client, m_impl->m_lastId++);

        return task;
    }


    ADatabaseSignals* DatabaseThread::notifier()
    {
        return &m_impl->m_executor;
    }


    void DatabaseThread::store(const Task& db_task, const PhotoInfo::Ptr& photo)
    {
        StoreTask* task = new StoreTask(db_task, photo);
        m_impl->addTask(task);
    }


    void DatabaseThread::getAllPhotos(const Task& db_task)
    {
        GetAllPhotosTask* task = new GetAllPhotosTask(db_task);
        m_impl->addTask(task);
    }


    void DatabaseThread::getPhoto(const Task& db_task, const PhotoInfo::Id& id)
    {
        GetPhotoTask* task = new GetPhotoTask(db_task, id);
        m_impl->addTask(task);
    }


    void DatabaseThread::getPhotos(const Task& db_task, const std::deque<IFilter::Ptr>& filter)
    {
        GetPhotosTask* task = new GetPhotosTask(db_task, filter);
        m_impl->addTask(task);
    }


    void DatabaseThread::listTags(const Task& db_task)
    {
        ListTagsTask* task = new ListTagsTask(db_task);
        m_impl->addTask(task);
    }


    void DatabaseThread::listTagValues(const Task& db_task, const TagNameInfo& info)
    {
        ListTagValuesTask* task = new ListTagValuesTask(db_task, info, std::deque<IFilter::Ptr>());
        m_impl->addTask(task);
    }


    void DatabaseThread::listTagValues(const Task& db_task, const TagNameInfo& info, const std::deque<IFilter::Ptr>& filter)
    {
        ListTagValuesTask* task = new ListTagValuesTask(db_task, info, filter);
        m_impl->addTask(task);
    }

}
