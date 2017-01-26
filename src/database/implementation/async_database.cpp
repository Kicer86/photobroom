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

#include "async_database.hpp"

#include <thread>
#include <memory>

#include <OpenLibrary/putils/ts_queue.hpp>

#include "ibackend.hpp"
#include "iphoto_info_cache.hpp"
#include "photo_data.hpp"
#include "photo_info.hpp"
#include "photo_info_storekeeper.hpp"
#include "project_info.hpp"


namespace
{
    struct IThreadVisitor;
    struct GetPhotosTask;
    struct ListTagsTask;
    struct ListTagValuesTask;
    struct AnyPhotoTask;
    struct DropPhotosTask;
    struct PerformActionTask;

    struct Executor;

    struct IThreadTask
    {
        virtual ~IThreadTask() {}
        virtual void visitMe(IThreadVisitor *) {}
        virtual void execute(Executor *) {}
    };

    struct ThreadBaseTask: IThreadTask
    {
        ThreadBaseTask() {}
        virtual ~ThreadBaseTask() {}
    };

    struct IThreadVisitor
    {
        virtual ~IThreadVisitor() {}

        virtual void visit( GetPhotosTask *) = 0;
        virtual void visit( ListTagsTask *) = 0;
        virtual void visit( ListTagValuesTask *) = 0;
        virtual void visit(AnyPhotoTask *) = 0;
        virtual void visit(DropPhotosTask *) = 0;
        virtual void visit(PerformActionTask *) = 0;
    };

    struct GetPhotosTask: ThreadBaseTask
    {
        GetPhotosTask (const std::deque<Database::IFilter::Ptr>& filter, const Database::IDatabase::Callback<const IPhotoInfo::List &>& callback):
            ThreadBaseTask(),
            m_filter(filter),
            m_callback(callback)
        {

        }

        virtual ~GetPhotosTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::deque<Database::IFilter::Ptr> m_filter;
        Database::IDatabase::Callback<const IPhotoInfo::List &> m_callback;
    };

    struct ListTagsTask: ThreadBaseTask
    {
        ListTagsTask (const Database::IDatabase::Callback<const std::deque<TagNameInfo> &> & callback):
            ThreadBaseTask(),
            m_callback(callback)
        {

        }

        virtual ~ListTagsTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        Database::IDatabase::Callback<const std::deque<TagNameInfo> &> m_callback;
    };

    struct ListTagValuesTask: ThreadBaseTask
    {
        ListTagValuesTask (const TagNameInfo& info,
                           const std::deque<Database::IFilter::Ptr>& filter,
                           const Database::IDatabase::Callback<const TagNameInfo &, const std::deque<TagValue> &> & callback):
        ThreadBaseTask(),
        m_callback(callback),
        m_info(info),
        m_filter(filter)
        {

        }

        virtual ~ListTagValuesTask() {}

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        const Database::IDatabase::Callback<const TagNameInfo &, const std::deque<TagValue> &> m_callback;
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

    struct DropPhotosTask: ThreadBaseTask
    {
        DropPhotosTask(std::unique_ptr<Database::ADropPhotosTask>&& task, const std::deque<Database::IFilter::Ptr>& filter):
            ThreadBaseTask(),
            m_task(std::move(task)),
            m_filter(filter)
        {

        }

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::unique_ptr<Database::ADropPhotosTask> m_task;
        std::deque<Database::IFilter::Ptr> m_filter;
    };

    struct PerformActionTask: ThreadBaseTask
    {
        PerformActionTask(const std::deque<Database::IFilter::Ptr>& filter, const std::deque<Database::IAction::Ptr>& action):
            ThreadBaseTask(),
            m_filter(filter),
            m_action(action)
        {
        }

        virtual void visitMe(IThreadVisitor* visitor) { visitor->visit(this); }

        std::deque<Database::IFilter::Ptr> m_filter;
        std::deque<Database::IAction::Ptr> m_action;
    };

    struct Executor: IThreadVisitor, Database::ADatabaseSignals
    {
        Executor( std::unique_ptr<Database::IBackend>&& backend, PhotoInfoStorekeeper* storekeeper):
            m_tasks(1024),
            m_backend( std::move(backend) ),
            m_cache(nullptr),
            m_storekeeper(storekeeper)
        {

        }

        Executor(const Executor &) = delete;
        Executor& operator=(const Executor &) = delete;

        virtual ~Executor() {}

        void set(Database::IPhotoInfoCache* cache)
        {
            m_cache = cache;
        }

        virtual void visit( GetPhotosTask* task) override
        {
            auto photos = m_backend->getPhotos(task->m_filter);
            IPhotoInfo::List photosList;

            for(const Photo::Id& id: photos)
                photosList.push_back(getPhotoFor(id));

            task->m_callback(photosList);
        }

        virtual void visit( ListTagsTask* task) override
        {
            auto result = m_backend->listTags();
            task->m_callback(result);
        }

        virtual void visit( ListTagValuesTask* task) override
        {
            auto result = m_backend->listTagValues(task->m_info, task->m_filter);
            task->m_callback(task->m_info, result);
        }

        virtual void visit(AnyPhotoTask* task) override
        {
            auto result = m_backend->getPhotosCount(task->m_filter);
            task->m_task->got(result);
        }

        virtual void visit(DropPhotosTask* task) override
        {
            auto photos = m_backend->dropPhotos(task->m_filter);

            task->m_task->got(photos);

            emit photosRemoved(photos);
        }

        virtual void visit(PerformActionTask* task) override
        {
            m_backend->perform(task->m_filter, task->m_action);
        }

        void begin()
        {
            for(;;)
            {
                ol::Optional< std::unique_ptr<ThreadBaseTask> > task = m_tasks.pop();

                if (task)
                {
                    ThreadBaseTask* baseTask = task->get();
                    baseTask->visitMe(this);
                    baseTask->execute(this);
                }
                else
                    break;
            }
        }

        void stop()
        {
            m_tasks.stop();
        }

        void closeConnections()
        {
            m_backend->closeConnections();
        }

        IPhotoInfo::Ptr constructPhotoInfo(const Photo::Data& data)
        {
            IPhotoInfo::Ptr photoInfo = std::make_shared<PhotoInfo>(data);

            m_cache->introduce(photoInfo);
            m_storekeeper->photoInfoConstructed(photoInfo);

            return photoInfo;
        }

        IPhotoInfo::Ptr getPhotoFor(const Photo::Id& id)
        {
            IPhotoInfo::Ptr photoPtr = m_cache->find(id);

            if (photoPtr.get() == nullptr)
            {
                const Photo::Data photoData = m_backend->getPhoto(id);

                photoPtr = constructPhotoInfo(photoData);
            }

            return photoPtr;
        }

        std::vector<Photo::Id> insertPhotos(const std::set<QString>& paths)
        {
            std::vector<Photo::Id> result;

            std::deque<Photo::Data> data_set;

            for(const QString& path: paths)
            {
                Photo::Data data;
                data.path = path;
                data.flags[Photo::FlagsE::StagingArea] = 1;

                data_set.push_back(data);
            }

            const bool status = m_backend->addPhotos(data_set);

            if (status)
            {
                std::deque<IPhotoInfo::Ptr> photos;

                for(std::size_t i = 0; i < data_set.size(); i++)
                {
                    const Photo::Data& data = data_set[i];
                    IPhotoInfo::Ptr photoInfo = constructPhotoInfo(data);
                    photos.push_back(photoInfo);

                    result.push_back(data.id);
                }

                emit photosAdded(photos);
            }

            return result;
        }

        Database::IBackend* getBackend() const
        {
            return m_backend.get();
        }

        void addTask(std::unique_ptr<ThreadBaseTask>&& task)
        {
            m_tasks.push(std::move(task));
        }

        private:
            ol::TS_Queue<std::unique_ptr<ThreadBaseTask>> m_tasks;
            std::unique_ptr<Database::IBackend> m_backend;
            Database::IPhotoInfoCache* m_cache;
            PhotoInfoStorekeeper* m_storekeeper;
    };

    struct CreateGroupTask: ThreadBaseTask
    {
        CreateGroupTask(const Photo::Id& representative, const std::function<void(Group::Id)>& callback):
            ThreadBaseTask(),
            m_representative(representative),
            m_callback(callback)
        {

        }

        virtual ~CreateGroupTask() {}

        virtual void execute(Executor* executor) override
        {
            const Group::Id id = executor->getBackend()->addGroup(m_representative);

            if (m_callback)
                m_callback(id);
        }

        Photo::Id m_representative;
        std::function<void(Group::Id)> m_callback;
    };

    struct GetPhotoTask: ThreadBaseTask
    {
        GetPhotoTask(const std::vector<Photo::Id>& ids, const std::function<void(const std::deque<IPhotoInfo::Ptr> &)>& callback):
            ThreadBaseTask(),
            m_ids(ids),
            m_callback(callback)
        {

        }

        virtual ~GetPhotoTask() {}

        virtual void execute(Executor* executor) override
        {
            std::deque<IPhotoInfo::Ptr> photos;

            for (const Photo::Id& id: m_ids)
            {
                IPhotoInfo::Ptr photo = executor->getPhotoFor(id);
                photos.push_back(photo);
            }

            m_callback(photos);
        }

        std::vector<Photo::Id> m_ids;
        std::function<void(const std::deque<IPhotoInfo::Ptr> &)> m_callback;
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

        virtual void execute(Executor* executor) override
        {
            Database::BackendStatus status = executor->getBackend()->init(m_prjInfo);
            m_task->got(status);
        }

        std::unique_ptr<Database::AInitTask> m_task;
        Database::ProjectInfo m_prjInfo;
    };


    struct InsertPhotosTask: ThreadBaseTask
    {
        InsertPhotosTask(const std::set<QString>& paths, const std::function<void(const std::vector<Photo::Id> &)>& callback):
            ThreadBaseTask(),
            m_paths(paths),
            m_callback(callback)
        {

        }

        virtual ~InsertPhotosTask() {}

        virtual void execute(Executor* executor) override
        {
            const std::vector<Photo::Id> result = executor->insertPhotos(m_paths);

            if (m_callback)
                m_callback(result);
        }

        std::set<QString> m_paths;
        std::function<void(const std::vector<Photo::Id> &)> m_callback;
    };


    struct UpdateTask: ThreadBaseTask
    {
        UpdateTask(std::unique_ptr<Database::AStorePhotoTask>&& task, const Photo::Data& photoData):
            ThreadBaseTask(),
            m_task(std::move(task)),
            m_photoData(photoData)
        {

        }

        virtual ~UpdateTask() {}

        virtual void execute(Executor* executor) override
        {
            const bool status = executor->getBackend()->update(m_photoData);
            m_task->got(status);

            IPhotoInfo::Ptr photoInfo = executor->getPhotoFor(m_photoData.id);
            emit executor->photoModified(photoInfo);
        }

        std::unique_ptr<Database::AStorePhotoTask> m_task;
        Photo::Data m_photoData;
    };

}


namespace Database
{

    struct AsyncDatabase::Impl
    {
        Impl( std::unique_ptr<IBackend>&& backend):
            m_cache(nullptr),
            m_storekeeper(),
            m_executor(std::move(backend), &m_storekeeper),
            m_thread(),
            m_working(true)
        {
            m_thread = std::thread(&Executor::begin, &m_executor);
        }

        //store task to be executed by thread
        void addTask(ThreadBaseTask* task)
        {
            assert(m_working);
            m_executor.addTask( std::move(std::unique_ptr<ThreadBaseTask>(task)) );
        }

        //store task to be executed by thread
        void addTask(std::unique_ptr<ThreadBaseTask>&& task)
        {
            assert(m_working);
            m_executor.addTask(std::move(task));
        }

        void stopExecutor()
        {
            if (m_working)
            {
                m_working = false;
                m_executor.stop();

                assert(m_thread.joinable());
                m_thread.join();

                m_executor.closeConnections();
            }
        }

        std::unique_ptr<IPhotoInfoCache> m_cache;
        PhotoInfoStorekeeper m_storekeeper;
        Executor m_executor;
        std::thread m_thread;
        bool m_working;
    };


    AsyncDatabase::AsyncDatabase ( std::unique_ptr<IBackend>&& backend ):
        m_impl(nullptr)
    {
        m_impl = std::make_unique<Impl>( std::move(backend));
        m_impl->m_storekeeper.setDatabase(this);
    }


    AsyncDatabase::~AsyncDatabase()
    {
        //terminate thread
        closeConnections();
    }


    void AsyncDatabase::closeConnections()
    {
        m_impl->stopExecutor();
    }



    void AsyncDatabase::set(std::unique_ptr<IPhotoInfoCache>&& cache)
    {
        m_impl->m_cache = std::move(cache);
        m_impl->m_executor.set(m_impl->m_cache.get());
        m_impl->m_storekeeper.setCache(m_impl->m_cache.get());
    }


    ADatabaseSignals* AsyncDatabase::notifier()
    {
        return &m_impl->m_executor;
    }


    void AsyncDatabase::exec(std::unique_ptr<Database::AInitTask>&& db_task, const Database::ProjectInfo& prjInfo)
    {
        InitTask* task = new InitTask(std::move(db_task), prjInfo);
        m_impl->addTask(task);
    }


    void AsyncDatabase::exec(std::unique_ptr<Database::AStorePhotoTask>&& db_task, const IPhotoInfo::Ptr& photo)
    {
        UpdateTask* task = new UpdateTask(std::move(db_task), photo->data());
        m_impl->addTask(task);
    }


    void AsyncDatabase::store(const std::set<QString>& paths, const Callback<const std::vector<Photo::Id> &>& callback)
    {
        InsertPhotosTask* task = new InsertPhotosTask(paths, callback);
        m_impl->addTask(task);
    }


    void AsyncDatabase::createGroup(const Photo::Id& id, const Callback<Group::Id>& callback)
    {
        CreateGroupTask* task = new CreateGroupTask(id, callback);
        m_impl->addTask(task);
    }


    void AsyncDatabase::exec(std::unique_ptr<Database::AGetPhotoTask>&& db_task, const Photo::Id& id)
    {
        std::shared_ptr<Database::AGetPhotoTask> db_task_shared(db_task.release());

        auto callback =
            [db_task_shared](const std::deque<IPhotoInfo::Ptr>& photos)
            {
                db_task_shared->got(photos);
            };

        GetPhotoTask* task = new GetPhotoTask({id}, callback);
        m_impl->addTask(task);
    }


    void AsyncDatabase::exec(std::unique_ptr<AGetPhotosCount>&& db_task, const std::deque<IFilter::Ptr>& filters)
    {
        AnyPhotoTask* task = new AnyPhotoTask(std::move(db_task), filters);
        m_impl->addTask(task);
    }


    void AsyncDatabase::getPhotos(const std::vector<Photo::Id>& ids, const Callback<std::deque<IPhotoInfo::Ptr>>& callback)
    {
        GetPhotoTask* task = new GetPhotoTask(ids, callback);
        m_impl->addTask(task);
    }


    void AsyncDatabase::listTagNames( const Callback<const std::deque<TagNameInfo> &> & callback)
    {
        ListTagsTask* task = new ListTagsTask(callback);
        m_impl->addTask(task);
    }

    void AsyncDatabase::listTagValues( const TagNameInfo& info, const Callback<const TagNameInfo &, const std::deque<TagValue> &> & callback)
    {
        ListTagValuesTask* task = new ListTagValuesTask (info, std::deque<IFilter::Ptr>(), callback);
        m_impl->addTask(task);
    }

    void AsyncDatabase::listTagValues( const TagNameInfo& info, const std::deque<IFilter::Ptr>& filters, const Callback<const TagNameInfo &, const std::deque<TagValue> &> & callback)
    {
        ListTagValuesTask* task = new ListTagValuesTask (info, filters, callback);
        m_impl->addTask(task);
    }


    void AsyncDatabase::listPhotos(const std::deque<IFilter::Ptr>& filter, const Callback<const IPhotoInfo::List &>& callback)
    {
        auto task = std::make_unique<GetPhotosTask>(filter, callback);
        m_impl->addTask(std::move(task));
    }


    void AsyncDatabase::perform(const std::deque<IFilter::Ptr>& filters, const std::deque<IAction::Ptr>& actions)
    {
        assert(!"bad implementation");
        PerformActionTask* task = new PerformActionTask(filters, actions);
        m_impl->addTask(task);
    }


    void AsyncDatabase::exec(std::unique_ptr<ADropPhotosTask>&& db_task , const std::deque<IFilter::Ptr>& filters)
    {
        DropPhotosTask* task = new DropPhotosTask(std::move(db_task), filters);
        m_impl->addTask(task);
    }

}
