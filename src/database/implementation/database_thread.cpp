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

#include <OpenLibrary/putils/ts_queue.hpp>

#include "ibackend.hpp"
#include "project_info.hpp"

namespace
{
    struct IThreadVisitor;
    struct InitTask;
    struct InsertTask;
    struct InsertTagTask;
    struct UpdateTask;
    struct GetAllPhotosTask;
    struct GetPhotoTask;
    struct GetPhotosTask;
    struct ListTagsTask;
    struct ListTagValuesTask;
    struct AnyPhotoTask;

    struct IThreadTask
    {
        virtual ~IThreadTask() {}
        virtual void visitMe(IThreadVisitor *) = 0;
    };

    struct ThreadBaseTask: IThreadTask
    {
        ThreadBaseTask() {}
        virtual ~ThreadBaseTask() {}
    };

    struct IThreadVisitor
    {
        virtual ~IThreadVisitor() {}

        virtual void visit(InitTask *) = 0;
        virtual void visit(InsertTask *) = 0;
        virtual void visit(UpdateTask *) = 0;
        virtual void visit(InsertTagTask *) = 0;
        virtual void visit(GetAllPhotosTask *) = 0;
        virtual void visit(GetPhotoTask *) = 0;
        virtual void visit(GetPhotosTask *) = 0;
        virtual void visit(ListTagsTask *) = 0;
        virtual void visit(ListTagValuesTask *) = 0;
        virtual void visit(AnyPhotoTask *) = 0;

    };

    struct InitTask: ThreadBaseTask
    {
        InitTask(std::unique_ptr<Database::AInitTask>&& task, const Database::ProjectInfo& prjInfo):
            ThreadBaseTask(),
            m_task(std::move(task)),
            m_prjInfo(prjInfo)
        {

        }

        virtual ~InitTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::unique_ptr<Database::AInitTask> m_task;
        Database::ProjectInfo m_prjInfo;
    };

    struct InsertTask: ThreadBaseTask
    {
        InsertTask(std::unique_ptr<Database::AStorePhotoTask>&& task, const QString& path):
            ThreadBaseTask(),
            m_task(std::move(task)),
            m_path(path)
        {

        }

        virtual ~InsertTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::unique_ptr<Database::AStorePhotoTask> m_task;
        QString m_path;
    };

    struct UpdateTask: ThreadBaseTask
    {
        UpdateTask(std::unique_ptr<Database::AStorePhotoTask>&& task, const IPhotoInfo::Ptr& photo):
            ThreadBaseTask(),
            m_task(std::move(task)),
            m_photoInfo(photo)
        {

        }

        virtual ~UpdateTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::unique_ptr<Database::AStorePhotoTask> m_task;
        IPhotoInfo::Ptr m_photoInfo;
    };


    struct InsertTagTask: ThreadBaseTask
    {
        InsertTagTask(std::unique_ptr<Database::AStoreTagTask>&& task, const TagNameInfo& tagInfo):
            ThreadBaseTask(),
            m_task(std::move(task)),
            m_tagInfo(tagInfo)
        {
        }

        virtual ~InsertTagTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::unique_ptr<Database::AStoreTagTask> m_task;
        TagNameInfo m_tagInfo;
    };


    struct GetAllPhotosTask: ThreadBaseTask
    {
        GetAllPhotosTask(std::unique_ptr<Database::AGetPhotosTask>&& task):
            ThreadBaseTask(),
            m_task(std::move(task))
        {

        }

        virtual ~GetAllPhotosTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::unique_ptr<Database::AGetPhotosTask> m_task;
    };


    struct GetPhotoTask: ThreadBaseTask
    {
        GetPhotoTask(std::unique_ptr<Database::AGetPhotoTask>&& task, const IPhotoInfo::Id& id):
            ThreadBaseTask(),
            m_task(std::move(task)),
            m_id(id)
        {

        }

        virtual ~GetPhotoTask() {}
        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::unique_ptr<Database::AGetPhotoTask> m_task;
        IPhotoInfo::Id m_id;
    };

    struct GetPhotosTask: ThreadBaseTask
    {
        GetPhotosTask(std::unique_ptr<Database::AGetPhotosTask>&& task, const std::deque<Database::IFilter::Ptr>& filter):
            ThreadBaseTask(),
            m_task(std::move(task)),
            m_filter(filter)
        {

        }

        virtual ~GetPhotosTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::unique_ptr<Database::AGetPhotosTask> m_task;
        std::deque<Database::IFilter::Ptr> m_filter;
    };

    struct ListTagsTask: ThreadBaseTask
    {
        ListTagsTask(std::unique_ptr<Database::AListTagsTask>&& task):
            ThreadBaseTask(),
            m_task(std::move(task))
        {

        }

        virtual ~ListTagsTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::unique_ptr<Database::AListTagsTask> m_task;
    };

    struct ListTagValuesTask: ThreadBaseTask
    {
        ListTagValuesTask(std::unique_ptr<Database::AListTagValuesTask>&& task, const TagNameInfo& info, const std::deque<Database::IFilter::Ptr>& filter):
            ThreadBaseTask(),
            m_task(std::move(task)),
            m_info(info),
            m_filter(filter)
        {

        }

        virtual ~ListTagValuesTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::unique_ptr<Database::AListTagValuesTask> m_task;
        TagNameInfo m_info;
        std::deque<Database::IFilter::Ptr> m_filter;
    };

    struct AnyPhotoTask: ThreadBaseTask
    {
        AnyPhotoTask(std::unique_ptr<Database::AGetPhotosCount>&& task, const std::deque<Database::IFilter::Ptr>& filter):
            ThreadBaseTask(),
            m_task(std::move(task)),
            m_filter(filter)
        {

        }

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::unique_ptr<Database::AGetPhotosCount> m_task;
        std::deque<Database::IFilter::Ptr> m_filter;
    };

    struct Executor: IThreadVisitor, Database::ADatabaseSignals
    {
        Executor(Database::IBackend* backend): m_backend(backend), m_tasks(1024) {}
        Executor(const Executor &) = delete;
        Executor& operator=(const Executor &) = delete;

        virtual ~Executor() {}

        virtual void visit(InitTask* task) override
        {
            m_backend->init(task->m_prjInfo);

            //TODO: result
        }

        virtual void visit(InsertTask* task) override
        {
            IPhotoInfo::Ptr photoInfo = m_backend->addPath(task->m_path);
            task->m_task->got(photoInfo.get() != nullptr);

            emit photoAdded(photoInfo);
        }

        virtual void visit(UpdateTask* task) override
        {
            const bool status = m_backend->update(task->m_photoInfo);
            task->m_task->got(status);

            emit photoModified(task->m_photoInfo);
        }

        virtual void visit(InsertTagTask* task) override
        {
            const bool status = m_backend->update(task->m_tagInfo);
            task->m_task->got(status);
        }

        virtual void visit(GetAllPhotosTask* task) override
        {
            auto result = m_backend->getAllPhotos();
            task->m_task->got(result);
        }

        virtual void visit(GetPhotoTask* task) override
        {
            auto result = m_backend->getPhoto(task->m_id);
            task->m_task->got(result);
        }

        virtual void visit(GetPhotosTask* task) override
        {
            auto result = m_backend->getPhotos(task->m_filter);
            task->m_task->got(result);
        }

        virtual void visit(ListTagsTask* task) override
        {
            auto result = m_backend->listTags();
            task->m_task->got(result);
        }

        virtual void visit(ListTagValuesTask* task) override
        {
            auto result = m_backend->listTagValues(task->m_info, task->m_filter);
            task->m_task->got(result);
        }

        virtual void visit(AnyPhotoTask* task) override
        {
            auto result = m_backend->getPhotosCount(task->m_filter);
            task->m_task->got(result);
        }

        void begin()
        {
            bool transation_opened = false;
            auto transaction_begin_time = std::chrono::steady_clock::now();

            for(;;)
            {
                ol::Optional< std::shared_ptr<ThreadBaseTask> > task = m_tasks.pop_front();

                if (task)
                {
                    //check if transactions are ready
                    const bool transactions_ready = m_backend->transactionsReady();

                    //begin transaction in not started yet
                    if (transactions_ready && transation_opened == false)
                    {
                        m_backend->beginTransaction();
                        transation_opened = true;
                        transaction_begin_time = std::chrono::steady_clock::now();
                    }

                    ThreadBaseTask* baseTask = task->get();
                    baseTask->visitMe(this);

                    //calculate how long transaction is active
                    const auto current_time = std::chrono::steady_clock::now();
                    const auto time_diff = current_time - transaction_begin_time;
                    const auto transaction_duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_diff).count();

                    //no more tasks or transaction takes too long? end transaction
                    if ( transation_opened && (transaction_duration > 2000 || m_tasks.empty()) )
                    {
                        m_backend->endTransaction();
                        transation_opened = false;
                    }
                }
                else
                    break;
            }
        }

        Database::IBackend* m_backend;
        ol::TS_Queue<std::shared_ptr<ThreadBaseTask>> m_tasks;
    };

}


namespace Database
{

    struct DatabaseThread::Impl
    {
        Impl(IBackend* backend): m_executor(backend), m_thread(beginThread, &m_executor), m_working(true)
        {

        }

        //store task to be executed by thread
        void addTask(ThreadBaseTask* task)
        {
            assert(m_working);
            m_executor.m_tasks.push_back(std::shared_ptr<ThreadBaseTask>(task));
        }

        static void beginThread(Executor* executor)
        {
            executor->begin();
        }

        void stopExecutor()
        {
            if (m_working)
            {
                m_working = false;
                m_executor.m_tasks.stop();

                assert(m_thread.joinable());
                m_thread.join();

                m_executor.m_backend->closeConnections();
            }
        }

        Executor m_executor;
        std::thread m_thread;
        bool m_working;
    };


    DatabaseThread::DatabaseThread(IBackend* backend): m_impl(new Impl(backend))
    {

    }


    DatabaseThread::~DatabaseThread()
    {
        //terminate thread
        closeConnections();
    }


    void DatabaseThread::closeConnections()
    {
        m_impl->stopExecutor();
    }


    bool DatabaseThread::exec(std::unique_ptr<Database::AInitTask>&& db_task, const Database::ProjectInfo& prjInfo)
    {
        InitTask* task = new InitTask(std::move(db_task), prjInfo);
        m_impl->addTask(task);

        //TODO: fix it
        return true;
    }


    ADatabaseSignals* DatabaseThread::notifier()
    {
        return &m_impl->m_executor;
    }


    void DatabaseThread::exec(std::unique_ptr<Database::AStorePhotoTask>&& db_task, const QString& path)
    {
        InsertTask* task = new InsertTask(std::move(db_task), path);
        m_impl->addTask(task);
    }


    void DatabaseThread::exec(std::unique_ptr<Database::AStorePhotoTask>&& db_task, const IPhotoInfo::Ptr& photo)
    {
        UpdateTask* task = new UpdateTask(std::move(db_task), photo);
        m_impl->addTask(task);
    }


    void DatabaseThread::exec(std::unique_ptr<AStoreTagTask>&& db_task, const TagNameInfo& tagNameInfo)
    {
        InsertTagTask* task = new InsertTagTask(std::move(db_task), tagNameInfo);
        m_impl->addTask(task);
    }


    void DatabaseThread::exec(std::unique_ptr<Database::AListTagsTask>&& db_task)
    {
        ListTagsTask* task = new ListTagsTask(std::move(db_task));
        m_impl->addTask(task);
    }


    void DatabaseThread::exec(std::unique_ptr<Database::AGetPhotoTask>&& db_task, const IPhotoInfo::Id& id)
    {
        GetPhotoTask* task = new GetPhotoTask(std::move(db_task), id);
        m_impl->addTask(task);
    }


    void DatabaseThread::exec(std::unique_ptr<Database::AGetPhotosTask>&& db_task, const std::deque<Database::IFilter::Ptr>& filter)
    {
        GetPhotosTask* task = new GetPhotosTask(std::move(db_task), filter);
        m_impl->addTask(task);
    }


    void DatabaseThread::exec(std::unique_ptr< Database::AGetPhotosTask >&& db_task)
    {
        GetAllPhotosTask* task = new GetAllPhotosTask(std::move(db_task));
        m_impl->addTask(task);
    }


    void DatabaseThread::exec(std::unique_ptr<Database::AListTagValuesTask>&& db_task, const TagNameInfo& info)
    {
        ListTagValuesTask* task = new ListTagValuesTask(std::move(db_task), info, std::deque<IFilter::Ptr>());
        m_impl->addTask(task);
    }


    void DatabaseThread::exec(std::unique_ptr<Database::AListTagValuesTask>&& db_task, const TagNameInfo& info, const std::deque<Database::IFilter::Ptr>& filter)
    {
        ListTagValuesTask* task = new ListTagValuesTask(std::move(db_task), info, filter);
        m_impl->addTask(task);
    }


    void DatabaseThread::exec(std::unique_ptr<AGetPhotosCount>&& db_task, const std::deque<IFilter::Ptr>& filters)
    {
        AnyPhotoTask* task = new AnyPhotoTask(std::move(db_task), filters);
        m_impl->addTask(task);
    }

}
