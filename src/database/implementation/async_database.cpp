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

#include <core/down_cast.hpp>

#include "ibackend.hpp"
#include "iphoto_info_cache.hpp"
#include "photo_data.hpp"
#include "photo_info.hpp"
#include "project_info.hpp"


namespace
{
    Photo::Data dataFromDelta(const Photo::DataDelta& delta)
    {
        assert(delta.getId().valid());

        Photo::Data data;

        data.id = delta.getId();

        if (delta.has(Photo::Field::Checksum))
            data.sha256Sum = delta.get<Photo::Field::Checksum>();

        if (delta.has(Photo::Field::Flags))
            data.flags = delta.get<Photo::Field::Flags>();

        if (delta.has(Photo::Field::Geometry))
            data.geometry = delta.get<Photo::Field::Geometry>();

        if (delta.has(Photo::Field::GroupInfo))
            data.groupInfo = delta.get<Photo::Field::GroupInfo>();

        if (delta.has(Photo::Field::Path))
            data.path = delta.get<Photo::Field::Path>();

        if (delta.has(Photo::Field::Tags))
            data.tags = delta.get<Photo::Field::Tags>();

        return data;
    }


    // TODO: replace all tasks with lambdas in AsyncDatabase methods
    struct Executor;

    struct IThreadTask
    {
        virtual ~IThreadTask() {}
        virtual void execute(Executor *) = 0;
    };


    template<typename T>
    struct GenericTask: IThreadTask
    {
        GenericTask(const T& callable): m_callable(callable) {}

        void execute(Executor* executor) override
        {
            m_callable(executor);
        }

        T m_callable;
    };


    // TODO: it seems to be useless it this form, reduce it.
    struct Executor: Database::IBackendOperator
    {
        Executor( std::unique_ptr<Database::IBackend>&& backend, Database::AsyncDatabase* storekeeper):
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

        // run in a db thread started by AsyncDatabase::Impl
        void begin()
        {
            for(;;)
            {
                std::optional< std::unique_ptr<IThreadTask> > task = m_tasks.pop();

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

        void closeConnections() override
        {
            m_backend->closeConnections();
        }

        IPhotoInfo::Ptr constructPhotoInfo(const Photo::Data& data)
        {
            auto photoInfo = std::make_shared<PhotoInfo>(data, m_storekeeper);

            m_cache->introduce(photoInfo);

            return photoInfo;
        }

        // IBackend && IBackendOperator

        bool addPhotos(std::vector<Photo::DataDelta>& d) override
        {
            return m_backend->addPhotos(d);
        }

        Group::Id addGroup(const Photo::Id &, GroupInfo::Type) override
        {
            assert(!"Not implemented");
            return Group::Id();
        }

        std::vector<Photo::Id> dropPhotos(const std::vector<Database::IFilter::Ptr> & )
        {
            assert(!"Not implemented");
            return {};
        }

        std::vector<Photo::Id> getAllPhotos() override
        {
            assert(!"Not implemented");
            return {};
        }

        Photo::Data getPhoto(const Photo::Id & ) override
        {
            assert(!"Not implemented");
            return {};
        }

        std::vector<Photo::Id> getPhotos(const std::vector<Database::IFilter::Ptr>& filter) override
        {
            return m_backend->getPhotos(filter);
        }

        int getPhotosCount(const std::vector<Database::IFilter::Ptr> &) override
        {
            assert(!"Not implemented");
            return -1;
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

        Database::BackendStatus init(const Database::ProjectInfo & ) override
        {
            assert(!"Not implemented");
            return Database::StatusCodes::OpenFailed;
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
                    Photo::Data data = dataFromDelta(data_set[i]);
                    IPhotoInfo::Ptr photoInfo = constructPhotoInfo(data);
                    photos.push_back(photoInfo);

                    result.push_back(data.id);
                }

                emit m_storekeeper->photosAdded(photos);
            }

            return result;
        }

        std::vector<PersonName> listPeople() override
        {
            return m_backend->listPeople();
        }

        std::vector<PersonInfo> listPeople(const Photo::Id& id) override
        {
            return m_backend->listPeople(id);
        }

        std::vector<TagNameInfo> listTags() override
        {
            assert(!"Not implemented");
            return {};
        }

        std::vector<TagValue> listTagValues(const TagNameInfo & , const std::vector<Database::IFilter::Ptr> & ) override
        {
            assert(!"Not implemented");
            return {};
        }

        PersonName person(const Person::Id& id) override
        {
            assert(id.valid());
            return m_backend->person(id);
        }

        void perform(const std::vector<Database::IFilter::Ptr> & , const std::vector<Database::IAction::Ptr> & ) override
        {
            assert(!"Not implemented");
        }

        std::vector<Photo::Id> markStagedAsReviewed() override
        {
            assert(!"Not implemented");
            return {};
        }

        Person::Id store(const PersonName& d) override
        {
            return m_backend->store(d);
        }

        PersonInfo::Id store(const PersonInfo& d) override
        {
            return m_backend->store(d);
        }

        void set(const Photo::Id& id, const QString& name, int value) override
        {
            m_backend->set(id, name, value);
        }

        std::optional<int> get(const Photo::Id& id, const QString& name) override
        {
            return m_backend->get(id, name);
        }

        bool update(const Photo::DataDelta &) override
        {
            assert(!"Not implemented");
            return false;
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

        template<typename T>
        void addTask(T&& callable)
        {
            std::unique_ptr<IThreadTask> task = std::make_unique<GenericTask<T>>(std::forward<T>(callable));
            addTask(std::move(task));
        }

        //private:
            ol::TS_Queue<std::unique_ptr<IThreadTask>> m_tasks;
            std::unique_ptr<Database::IBackend> m_backend;
            Database::IPhotoInfoCache* m_cache;
            Database::AsyncDatabase* m_storekeeper;
    };


    struct CustomAction: IThreadTask
    {
        CustomAction(std::unique_ptr<Database::IDatabase::ITask>&& operation): m_operation(std::move(operation))
        {

        }

        virtual void execute(Executor* executor) override
        {
            m_operation->run(executor);
        }

        std::unique_ptr<Database::IDatabase::ITask> m_operation;
    };
}


namespace Database
{
    // TODO: move methods implementation to AsyncDatabase. No need to keep them here
    struct AsyncDatabase::Impl
    {
        Impl(std::unique_ptr<IBackend>&& backend, AsyncDatabase* storekeeper):
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
            addTask(std::unique_ptr<IThreadTask>(task));
        }

        //store task to be executed by thread
        void addTask(std::unique_ptr<IThreadTask>&& task)
        {
            // When task comes from from db's thread execute it immediately.
            // This simplifies some client's codes (when operating inside of execute())
            if (std::this_thread::get_id() == m_thread.get_id())
                task->execute(&m_executor);
            else
            {
                assert(m_working);
                m_executor.addTask(std::move(task));
            }
        }

        void stopExecutor()
        {
            if (m_working)
            {
                // do not accept any more tasks
                m_working = false;

                // add final task
                m_executor.addTask([](Executor* executor)
                {
                    executor->closeConnections();
                });
                m_executor.stop();

                // wait for all tasks to be finished
                assert(m_thread.joinable());
                m_thread.join();
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


    void AsyncDatabase::init(const ProjectInfo& prjInfo, const Callback<const BackendStatus &>& callback)
    {
        exec([prjInfo, callback](IBackendOperator* op)
        {
             Executor* ex = down_cast<Executor *>(op);
             const Database::BackendStatus status = ex->getBackend()->init(prjInfo);

             callback(status);
        });
    }


    void AsyncDatabase::update(const Photo::DataDelta& data)
    {
        exec([this, data](IBackendOperator* op)
        {
            Executor* ex = down_cast<Executor *>(op);
            const bool status = ex->getBackend()->update(data);
            assert(status);

            IPhotoInfo::Ptr photoInfo = ex->getPhotoFor(data.getId());
            emit photoModified(photoInfo);
        });
    }


    void AsyncDatabase::store(const std::vector<Photo::DataDelta>& photos, const Callback<const std::vector<Photo::Id> &>& callback)
    {
        exec([photos, callback](IBackendOperator* op)
        {
             Executor* ex = down_cast<Executor *>(op);
             const std::vector<Photo::Id> status = ex->insertPhotos(photos);

             callback(status);
        });
    }


    void AsyncDatabase::createGroup(const Photo::Id& id, GroupInfo::Type type, const Callback<Group::Id>& callback)
    {
        exec([id, type, callback](IBackendOperator* op)
        {
            Executor* ex = down_cast<Executor *>(op);
            const Group::Id gid = ex->getBackend()->addGroup(id, type);

            // mark representative as representative
            IPhotoInfo::Ptr representative = ex->getPhotoFor(id);
            const GroupInfo grpInfo = GroupInfo(gid, GroupInfo::Representative, type);
            representative->setGroup(grpInfo);

            callback(gid);
        });
    }


    void AsyncDatabase::countPhotos(const std::vector<IFilter::Ptr>& filters, const Callback<int>& callback)
    {
        exec([filters, callback](IBackendOperator* op)
        {
             Executor* ex = down_cast<Executor *>(op);
             const auto result = ex->getBackend()->getPhotosCount(filters);

             callback(result);
        });
    }


    void AsyncDatabase::getPhotos(const std::vector<Photo::Id>& ids, const Callback<const std::vector<IPhotoInfo::Ptr> &>& callback)
    {
        exec([ids, callback](IBackendOperator* op)
        {
            Executor* ex = down_cast<Executor *>(op);
            std::vector<IPhotoInfo::Ptr> photos;

            for (const Photo::Id& id: ids)
            {
                IPhotoInfo::Ptr photo = ex->getPhotoFor(id);
                photos.push_back(photo);
            }

            callback(photos);
        });
    }


    void AsyncDatabase::listTagNames( const Callback<const std::vector<TagNameInfo> &> & callback)
    {
        exec([callback](IBackendOperator* op)
        {
             Executor* ex = down_cast<Executor *>(op);
             const auto result = ex->getBackend()->listTags();

             callback(result);
        });
    }


    void AsyncDatabase::listTagValues(const TagNameInfo& info, const Callback<const TagNameInfo &, const std::vector<TagValue> &> & callback)
    {
        listTagValues(info, {}, callback);
    }


    void AsyncDatabase::listTagValues(const TagNameInfo& info, const std::vector<IFilter::Ptr>& filters, const Callback<const TagNameInfo &, const std::vector<TagValue> &> & callback)
    {
        exec([info, filters, callback](IBackendOperator* op)
        {
             Executor* ex = down_cast<Executor *>(op);
             const auto result = ex->getBackend()->listTagValues(info, filters);

             callback(info, result);
        });
    }


    void AsyncDatabase::listPhotos(const std::vector<IFilter::Ptr>& filter, const Callback<const IPhotoInfo::List &>& callback)
    {
        exec([filter, callback](IBackendOperator* op)
        {
            Executor* ex = down_cast<Executor *>(op);
            auto photos = ex->getBackend()->getPhotos(filter);
            IPhotoInfo::List photosList;

            for(const Photo::Id& id: photos)
                photosList.push_back(ex->getPhotoFor(id));

            callback(photosList);
        });
    }


    void AsyncDatabase::markStagedAsReviewed()
    {
        exec([this](IBackendOperator* op)
        {
            Executor* ex = down_cast<Executor *>(op);
            const std::vector<Photo::Id> moved = ex->getBackend()->markStagedAsReviewed();

            emit photosMarkedAsReviewed(moved);
        });
    }


    void AsyncDatabase::execute(std::unique_ptr<ITask>&& action)
    {
        auto task = std::make_unique<CustomAction>(std::move(action));
        m_impl->addTask(std::move(task));
    }

}
