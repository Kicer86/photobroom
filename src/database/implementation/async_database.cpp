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
#include "project_info.hpp"


namespace
{

    struct Executor;

    struct IThreadTask
    {
        virtual ~IThreadTask() {}
        virtual void execute(Executor *) = 0;
    };

    struct Executor: Database::ADatabaseSignals, Database::IBackendOperator
    {
        Executor( std::unique_ptr<Database::IBackend>&& backend, IPhotoInfoStorekeeper* storekeeper):
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

        void begin()
        {
            for(;;)
            {
                ol::Optional< std::unique_ptr<IThreadTask> > task = m_tasks.pop();

                if (task)
                {
                    IThreadTask* baseTask = task->get();
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

        template<typename T>
        IPhotoInfo::Ptr constructPhotoInfo(const T& data)
        {
            auto photoInfo = std::make_shared<PhotoInfo>(data, m_storekeeper);

            m_cache->introduce(photoInfo);

            return photoInfo;
        }

        // IBackendOperator

        std::vector<Photo::Id> getPhotos(const std::vector<Database::IFilter::Ptr>& filter) override
        {
            return m_backend->getPhotos(filter);
        }

        IPhotoInfo::Ptr getPhotoFor(const Photo::Id& id) override
        {
            IPhotoInfo::Ptr photoPtr = m_cache->find(id);

            if (photoPtr.get() == nullptr)
            {
                const Photo::Data photoData = m_backend->getPhoto(id);

                photoPtr = constructPhotoInfo(photoData);
            }

            return photoPtr;
        }

        std::vector<Photo::Id> insertPhotos(const std::vector<Photo::DataDelta>& dataDelta) override
        {
            std::vector<Photo::Id> result;

            std::vector<Photo::DataDelta> data_set(dataDelta.begin(), dataDelta.end());
            const bool status = m_backend->addPhotos(data_set);

            if (status)
            {
                std::vector<IPhotoInfo::Ptr> photos;

                for(std::size_t i = 0; i < data_set.size(); i++)
                {
                    const Photo::DataDelta& data = data_set[i];
                    IPhotoInfo::Ptr photoInfo = constructPhotoInfo(data);
                    photos.push_back(photoInfo);

                    result.push_back(data.id);
                }

                emit photosAdded(photos);
            }

            return result;
        }

        //

        Database::IBackend* getBackend() const
        {
            return m_backend.get();
        }

        void addTask(std::unique_ptr<IThreadTask>&& task)
        {
            m_tasks.push(std::move(task));
        }

        private:
            ol::TS_Queue<std::unique_ptr<IThreadTask>> m_tasks;
            std::unique_ptr<Database::IBackend> m_backend;
            Database::IPhotoInfoCache* m_cache;
            IPhotoInfoStorekeeper* m_storekeeper;
    };


    struct CreateGroupTask: IThreadTask
    {
        CreateGroupTask(const Photo::Id& representative, const std::function<void(Group::Id)>& callback):
            IThreadTask(),
            m_representative(representative),
            m_callback(callback)
        {

        }

        virtual ~CreateGroupTask() {}

        virtual void execute(Executor* executor) override
        {
            const Group::Id gid = executor->getBackend()->addGroup(m_representative);

            // mark representative as representative
            IPhotoInfo::Ptr representative = executor->getPhotoFor(m_representative);
            const GroupInfo grpInfo = GroupInfo( gid, GroupInfo::Representative);
            representative->setGroup(grpInfo);

            if (m_callback)
                m_callback( gid );
        }

        Photo::Id m_representative;
        std::function<void(Group::Id)> m_callback;
    };


    struct CustomAction: IThreadTask
    {
        CustomAction(const std::function<void(Database::IBackendOperator *)>& operation): m_operation(operation)
        {

        }

        virtual void execute(Executor* executor) override
        {
            m_operation(executor);
        }

        std::function<void(Database::IBackendOperator *)> m_operation;
    };


    struct GetPhotoTask: IThreadTask
    {
        GetPhotoTask(const std::vector<Photo::Id>& ids, const std::function<void(const std::vector<IPhotoInfo::Ptr> &)>& callback):
            IThreadTask(),
            m_ids(ids),
            m_callback(callback)
        {

        }

        virtual ~GetPhotoTask() {}

        virtual void execute(Executor* executor) override
        {
            std::vector<IPhotoInfo::Ptr> photos;

            for (const Photo::Id& id: m_ids)
            {
                IPhotoInfo::Ptr photo = executor->getPhotoFor(id);
                photos.push_back(photo);
            }

            m_callback(photos);
        }

        std::vector<Photo::Id> m_ids;
        std::function<void(const std::vector<IPhotoInfo::Ptr> &)> m_callback;
    };


    struct GetPhotosTask: IThreadTask
    {
        GetPhotosTask (const std::vector<Database::IFilter::Ptr>& filter, const Database::IDatabase::Callback<const IPhotoInfo::List &>& callback):
            IThreadTask(),
            m_filter(filter),
            m_callback(callback)
        {

        }

        virtual ~GetPhotosTask() {}

        virtual void execute(Executor* executor) override
        {
            auto photos = executor->getBackend()->getPhotos(m_filter);
            IPhotoInfo::List photosList;

            for(const Photo::Id& id: photos)
                photosList.push_back(executor->getPhotoFor(id));

            m_callback(photosList);
        }


        std::vector<Database::IFilter::Ptr> m_filter;
        Database::IDatabase::Callback<const IPhotoInfo::List &> m_callback;
    };


    struct InitTask: IThreadTask
    {
        InitTask(const Database::ProjectInfo& prjInfo, const std::function<void(const Database::BackendStatus &)>& callback):
            IThreadTask(),
            m_callback(callback),
            m_prjInfo(prjInfo)
        {

        }

        virtual ~InitTask() {}

        virtual void execute(Executor* executor) override
        {
            const Database::BackendStatus status = executor->getBackend()->init(m_prjInfo);
            m_callback(status);
        }

        std::function<void(const Database::BackendStatus &)> m_callback;
        Database::ProjectInfo m_prjInfo;
    };


    struct InsertPhotosTask: IThreadTask
    {
        InsertPhotosTask(const std::vector<Photo::DataDelta>& data, const std::function<void(const std::vector<Photo::Id> &)>& callback):
            IThreadTask(),
            m_data(data),
            m_callback(callback)
        {

        }

        virtual ~InsertPhotosTask() {}

        virtual void execute(Executor* executor) override
        {
            const std::vector<Photo::Id> result = executor->insertPhotos(m_data);

            if (m_callback)
                m_callback(result);
        }

        const std::vector<Photo::DataDelta> m_data;
        const std::function<void(const std::vector<Photo::Id> &)> m_callback;
    };

    struct ListTagValuesTask: IThreadTask
    {
        ListTagValuesTask (const TagNameInfo& info,
                           const std::vector<Database::IFilter::Ptr>& filter,
                           const Database::IDatabase::Callback<const TagNameInfo &, const std::vector<TagValue> &> & callback):
        IThreadTask(),
        m_callback(callback),
        m_info(info),
        m_filter(filter)
        {

        }

        virtual ~ListTagValuesTask() {}

        virtual void execute(Executor* executor) override
        {
            auto result = executor->getBackend()->listTagValues(m_info, m_filter);
            m_callback(m_info, result);
        }

        const Database::IDatabase::Callback<const TagNameInfo &, const std::vector<TagValue> &> m_callback;
        TagNameInfo m_info;
        std::vector<Database::IFilter::Ptr> m_filter;
    };


    struct ListTagsTask: IThreadTask
    {
        ListTagsTask (const Database::IDatabase::Callback<const std::vector<TagNameInfo> &> & callback):
            IThreadTask(),
            m_callback(callback)
        {

        }

        virtual ~ListTagsTask() {}

        virtual void execute(Executor* executor) override
        {
            auto result = executor->getBackend()->listTags();
            m_callback(result);
        }

        Database::IDatabase::Callback<const std::vector<TagNameInfo> &> m_callback;
    };


    struct PhotoCountTask: IThreadTask
    {
        PhotoCountTask (const std::vector<Database::IFilter::Ptr>& filter, const std::function<void(int)>& callback):
            IThreadTask(),
            m_callback(callback),
            m_filter(filter)
        {

        }

        virtual void execute(Executor* executor) override
        {
            auto result = executor->getBackend()->getPhotosCount(m_filter);
            m_callback(result);
        }

        std::function<void(int)> m_callback;
        std::vector<Database::IFilter::Ptr> m_filter;
    };


    struct UpdateTask: IThreadTask
    {
        UpdateTask(const Photo::DataDelta& photoData):
            IThreadTask(),
            m_photoData(photoData)
        {

        }

        virtual ~UpdateTask() {}

        virtual void execute(Executor* executor) override
        {
            const bool status = executor->getBackend()->update(m_photoData);
            assert(status);

            IPhotoInfo::Ptr photoInfo = executor->getPhotoFor(m_photoData.id);
            emit executor->photoModified(photoInfo);
        }

        Photo::DataDelta m_photoData;
    };

}


namespace Database
{

    struct AsyncDatabase::Impl
    {
        Impl(std::unique_ptr<IBackend>&& backend, IPhotoInfoStorekeeper* storekeeper):
            m_cache(nullptr),
            m_executor(std::move(backend), storekeeper),
            m_thread(),
            m_working(true)
        {
            m_thread = std::thread(&Executor::begin, &m_executor);
        }

        //store task to be executed by thread
        [[deprecated]] void addTask(IThreadTask* task)
        {
            assert(m_working);
            m_executor.addTask( std::move(std::unique_ptr<IThreadTask>(task)) );
        }

        //store task to be executed by thread
        void addTask(std::unique_ptr<IThreadTask>&& task)
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
        Executor m_executor;
        std::thread m_thread;
        bool m_working;
    };


    AsyncDatabase::AsyncDatabase(std::unique_ptr<IBackend>&& backend):
        m_impl(nullptr)
    {
        m_impl = std::make_unique<Impl>(std::move(backend), this);
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
    }


    ADatabaseSignals* AsyncDatabase::notifier()
    {
        return &m_impl->m_executor;
    }

    void AsyncDatabase::init(const ProjectInfo& prjInfo, const Callback<const BackendStatus &>& callback)
    {
        InitTask* task = new InitTask(prjInfo, callback);
        m_impl->addTask(task);
    }

    void AsyncDatabase::update(const Photo::DataDelta& data)
    {
        assert(data.id.valid());

        UpdateTask* task = new UpdateTask(data);
        m_impl->addTask(task);
    }

    void AsyncDatabase::store(const std::vector<Photo::DataDelta>& photos, const Callback<const std::vector<Photo::Id> &>& callback)
    {
        auto task = std::make_unique<InsertPhotosTask>(photos, callback);
        m_impl->addTask(std::move(task));
    }


    void AsyncDatabase::createGroup(const Photo::Id& id, const Callback<Group::Id>& callback)
    {
        CreateGroupTask* task = new CreateGroupTask(id, callback);
        m_impl->addTask(task);
    }


    void AsyncDatabase::countPhotos(const std::vector<IFilter::Ptr>& filters, const Callback<int>& callback)
    {
        PhotoCountTask* task = new PhotoCountTask(filters, callback);
        m_impl->addTask(task);
    }


    void AsyncDatabase::getPhotos(const std::vector<Photo::Id>& ids, const Callback<const std::vector<IPhotoInfo::Ptr> &>& callback)
    {
        GetPhotoTask* task = new GetPhotoTask(ids, callback);
        m_impl->addTask(task);
    }


    void AsyncDatabase::listTagNames( const Callback<const std::vector<TagNameInfo> &> & callback)
    {
        ListTagsTask* task = new ListTagsTask(callback);
        m_impl->addTask(task);
    }


    void AsyncDatabase::listTagValues( const TagNameInfo& info, const Callback<const TagNameInfo &, const std::vector<TagValue> &> & callback)
    {
        ListTagValuesTask* task = new ListTagValuesTask (info, std::vector<IFilter::Ptr>(), callback);
        m_impl->addTask(task);
    }


    void AsyncDatabase::listTagValues( const TagNameInfo& info, const std::vector<IFilter::Ptr>& filters, const Callback<const TagNameInfo &, const std::vector<TagValue> &> & callback)
    {
        ListTagValuesTask* task = new ListTagValuesTask (info, filters, callback);
        m_impl->addTask(task);
    }


    void AsyncDatabase::listPhotos(const std::vector<IFilter::Ptr>& filter, const Callback<const IPhotoInfo::List &>& callback)
    {
        auto task = std::make_unique<GetPhotosTask>(filter, callback);
        m_impl->addTask(std::move(task));
    }


    void AsyncDatabase::performCustomAction(const std::function<void(IBackendOperator *)>& action)
    {
        auto task = std::make_unique<CustomAction>(action);
        m_impl->addTask(std::move(task));
    }

}
