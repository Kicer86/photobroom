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
}


namespace Database
{
    struct IThreadTask
    {
        virtual ~IThreadTask() {}
        virtual void execute(IBackend *) = 0;
    };

    struct Executor
    {
        Executor( std::unique_ptr<Database::IBackend>&& backend, Database::AsyncDatabase* storekeeper):
            m_tasks(1024),
            m_backend(std::move(backend)),
            m_storekeeper(storekeeper)
        {

        }

        Executor(const Executor &) = delete;
        Executor& operator=(const Executor &) = delete;

        virtual ~Executor() {}

        // run in a db thread started by AsyncDatabase::Impl
        void begin()
        {
            for(;;)
            {
                std::optional< std::unique_ptr<IThreadTask> > task = m_tasks.pop();

                if (task)
                {
                    IThreadTask* baseTask = task->get();
                    baseTask->execute(m_backend.get());
                }
                else
                    break;
            }
        }

        void stop()
        {
            m_tasks.stop();
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

        //private:
            ol::TS_Queue<std::unique_ptr<IThreadTask>> m_tasks;
            std::unique_ptr<Database::IBackend> m_backend;
            Database::AsyncDatabase* m_storekeeper;
    };


    struct CustomAction: IThreadTask
    {
        CustomAction(std::unique_ptr<Database::IDatabase::ITask>&& operation): m_operation(std::move(operation))
        {

        }

        virtual void execute(IBackend* backend) override
        {
            m_operation->run(backend);
        }

        std::unique_ptr<Database::IDatabase::ITask> m_operation;
    };


    struct DbCloseTask: IThreadTask
    {
        DbCloseTask() = default;

        void execute(IBackend* backend) override
        {
           backend->closeConnections();
        }
    };


    AsyncDatabase::AsyncDatabase(std::unique_ptr<IBackend>&& backend):
        m_executor(std::make_unique<Executor>(std::move(backend), this)),
        m_working(true)
    {
        m_thread = std::thread(&Executor::begin, m_executor.get());
    }


    AsyncDatabase::~AsyncDatabase()
    {
        //terminate thread
        closeConnections();
    }


    void AsyncDatabase::closeConnections()
    {
        stopExecutor();
    }


    void AsyncDatabase::set(std::unique_ptr<IPhotoInfoCache>&& cache)
    {
        m_cache = std::move(cache);
    }


    void AsyncDatabase::init(const ProjectInfo& prjInfo, const Callback<const BackendStatus &>& callback)
    {
        exec([prjInfo, callback](IBackend* backend)
        {
             const Database::BackendStatus status = backend->init(prjInfo);

             callback(status);
        });
    }


    void AsyncDatabase::update(const Photo::DataDelta& data)
    {
        exec([this, data](IBackend* backend)
        {
            const bool status = backend->update(data);
            assert(status);

            IPhotoInfo::Ptr photoInfo = getPhotoFor(data.getId());
            emit photoModified(photoInfo);
        });
    }


    void AsyncDatabase::store(const std::vector<Photo::DataDelta>& photos, const Callback<const std::vector<Photo::Id> &>& callback)
    {
        exec([this, photos, callback](IBackend *)
        {
             const std::vector<Photo::Id> status = insertPhotos(photos);

             callback(status);
        });
    }


    void AsyncDatabase::createGroup(const Photo::Id& id, GroupInfo::Type type, const Callback<Group::Id>& callback)
    {
        exec([this, id, type, callback](IBackend* backend)
        {
            const Group::Id gid = backend->addGroup(id, type);

            // mark representative as representative
            IPhotoInfo::Ptr representative = getPhotoFor(id);
            const GroupInfo grpInfo = GroupInfo(gid, GroupInfo::Representative, type);
            representative->setGroup(grpInfo);

            callback(gid);
        });
    }


    void AsyncDatabase::countPhotos(const std::vector<IFilter::Ptr>& filters, const Callback<int>& callback)
    {
        exec([filters, callback](IBackend* backend)
        {
             const auto result = backend->getPhotosCount(filters);

             callback(result);
        });
    }


    void AsyncDatabase::getPhotos(const std::vector<Photo::Id>& ids, const Callback<const std::vector<IPhotoInfo::Ptr> &>& callback)
    {
        exec([this, ids, callback](IBackend *)
        {
            std::vector<IPhotoInfo::Ptr> photos;

            for (const Photo::Id& id: ids)
            {
                IPhotoInfo::Ptr photo = getPhotoFor(id);
                photos.push_back(photo);
            }

            callback(photos);
        });
    }


    void AsyncDatabase::listTagNames( const Callback<const std::vector<TagNameInfo> &> & callback)
    {
        exec([callback](IBackend* backend)
        {
             const auto result = backend->listTags();

             callback(result);
        });
    }


    void AsyncDatabase::listTagValues(const TagNameInfo& info, const Callback<const TagNameInfo &, const std::vector<TagValue> &> & callback)
    {
        listTagValues(info, {}, callback);
    }


    void AsyncDatabase::listTagValues(const TagNameInfo& info, const std::vector<IFilter::Ptr>& filters, const Callback<const TagNameInfo &, const std::vector<TagValue> &> & callback)
    {
        exec([info, filters, callback](IBackend* backend)
        {
             const auto result = backend->listTagValues(info, filters);

             callback(info, result);
        });
    }


    void AsyncDatabase::listPhotos(const std::vector<IFilter::Ptr>& filter, const Callback<const IPhotoInfo::List &>& callback)
    {
        exec([this, filter, callback](IBackend* backend)
        {
            auto photos = backend->getPhotos(filter);
            IPhotoInfo::List photosList;

            for(const Photo::Id& id: photos)
                photosList.push_back(getPhotoFor(id));

            callback(photosList);
        });
    }


    void AsyncDatabase::markStagedAsReviewed()
    {
        exec([this](IBackend* backend)
        {
            const std::vector<Photo::Id> moved = backend->markStagedAsReviewed();

            emit photosMarkedAsReviewed(moved);
        });
    }


    void AsyncDatabase::execute(std::unique_ptr<ITask>&& action)
    {
        auto task = std::make_unique<CustomAction>(std::move(action));
        addTask(std::move(task));
    }


    Database::IUtils* AsyncDatabase::utils()
    {
        return this;
    }


    void AsyncDatabase::addTask(std::unique_ptr<IThreadTask>&& task)
    {
        // When task comes from from db's thread execute it immediately.
        // This simplifies some client's codes (when operating inside of execute())
        if (std::this_thread::get_id() == m_thread.get_id())
            task->execute(m_executor->getBackend());
        else
        {
            assert(m_working);
            m_executor->addTask(std::move(task));
        }
    }


    void AsyncDatabase::stopExecutor()
    {
        if (m_working)
        {
            // do not accept any more tasks
            m_working = false;

            // add final task
            m_executor->addTask(std::make_unique<DbCloseTask>());
            m_executor->stop();

            // wait for all tasks to be finished
            assert(m_thread.joinable());
            m_thread.join();
        }
    }


    IPhotoInfo::Ptr AsyncDatabase::constructPhotoInfo(const Photo::Data& data)
    {
        auto photoInfo = std::make_shared<PhotoInfo>(data, this);

        m_cache->introduce(photoInfo);

        return photoInfo;
    }


    IPhotoInfo::Ptr AsyncDatabase::getPhotoFor(const Photo::Id& id)
    {
        IPhotoInfo::Ptr photoPtr = m_cache->find(id);

        if (photoPtr.get() == nullptr)
        {
            const Photo::Data photoData = m_executor->getBackend()->getPhoto(id);

            photoPtr = constructPhotoInfo(photoData);
        }

        return photoPtr;
    }


    std::vector<Photo::Id> AsyncDatabase::insertPhotos(const std::vector<Photo::DataDelta>& dataDelta)
    {
        std::vector<Photo::Id> result;

        std::vector<Photo::DataDelta> data_set(dataDelta.begin(), dataDelta.end());
        const bool status = m_executor->getBackend()->addPhotos(data_set);

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

            emit photosAdded(photos);
        }

        return result;
    }

}
