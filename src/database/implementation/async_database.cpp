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
#include <core/logger_factory.hpp>
#include <core/thread_utils.hpp>

#include "ibackend.hpp"
#include "igroup_operator.hpp"
#include "iphoto_info_cache.hpp"
#include "photo_data.hpp"
#include "photo_info.hpp"
#include "project_info.hpp"



namespace Database
{
    struct IThreadTask
    {
        virtual ~IThreadTask() {}
        virtual void execute(IBackend *) = 0;
    };

    struct Executor
    {
        Executor(Database::IBackend* backend):
            m_tasks(1024),
            m_backend(backend)
        {

        }

        Executor(const Executor &) = delete;
        Executor& operator=(const Executor &) = delete;

        virtual ~Executor() {}

        // run in a db thread started by AsyncDatabase::Impl
        void begin()
        {
            set_thread_name("ADatabase");

            for(;;)
            {
                std::optional< std::unique_ptr<IThreadTask> > task = m_tasks.pop();

                if (task)
                {
                    IThreadTask* baseTask = task->get();
                    baseTask->execute(m_backend);
                }
                else
                    break;
            }
        }

        void stop()
        {
            m_tasks.stop();
        }


        void addTask(std::unique_ptr<IThreadTask>&& task)
        {
            m_tasks.push(std::move(task));
        }

        private:
            ol::TS_Queue<std::unique_ptr<IThreadTask>> m_tasks;
            Database::IBackend* m_backend;
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


    ///////////////////////////////////////////////////////////////////////////


    Utils::Utils(IPhotoInfoCache* cache, IBackend* backend, IDatabase* keeper, ILogger* logger):
        m_logger(logger->subLogger("Utils")),
        m_cache(cache),
        m_backend(backend),
        m_storeKeeper(keeper)
    {
        QObject::connect(backend, &IBackend::photoModified, this, &Utils::photoModified, Qt::DirectConnection);
    }


    Utils::~Utils()
    {

    }


    IPhotoInfo::Ptr Utils::getPhotoFor(const Photo::Id& id)
    {
        IPhotoInfo::Ptr photoPtr = findInCache(id);

        if (photoPtr.get() == nullptr)
        {
            const Photo::Data photoData = m_backend->getPhoto(id);

            photoPtr = constructPhotoInfo(photoData);
        }

        return photoPtr;
    }


    std::vector<Photo::Id> Utils::insertPhotos(const std::vector<Photo::DataDelta>& dataDelta)
    {
        std::vector<Photo::Id> result;

        std::vector<Photo::DataDelta> data_set = dataDelta;
        const bool status = m_backend->addPhotos(data_set);

        assert(status);

        if (status)
            for(std::size_t i = 0; i < data_set.size(); i++)
                result.push_back(data_set[i].getId());

        return result;
    }


    IPhotoInfo::Ptr Utils::constructPhotoInfo(const Photo::Data& data)
    {
        auto photoInfo = std::make_shared<PhotoInfo>(data, m_storeKeeper);

        m_cache->introduce(photoInfo);

        const std::string insert_msg = std::string("Adding photo with id ") + std::to_string(data.id) + " to cache";
        m_logger->debug(insert_msg);

        return photoInfo;
    }


    void Utils::photoModified(const Photo::Id& id)
    {
        auto photoInfo = findInCache(id);

        if (photoInfo.get() != nullptr)
        {
            const Photo::Data photoData = m_backend->getPhoto(id);

            photoInfo->setData(photoData);
        }
    }


    IPhotoInfo::Ptr Utils::findInCache(const Photo::Id& id)
    {
        const std::string search_msg = std::string("Looking for photo with id ") + std::to_string(id) + " in cache";
        m_logger->debug(search_msg);

        auto photoInfo = m_cache->find(id);

        if (photoInfo.get() == nullptr)
        {
            const std::string result_msg = std::string("Photo with id ") + std::to_string(id) + " not found in cache";
            m_logger->debug(result_msg);
        }
        else
        {
            const std::string result_msg = std::string("Photo with id ") + std::to_string(id) + " found in cache";
            m_logger->debug(result_msg);
        }

        return photoInfo;
    }


    ///////////////////////////////////////////////////////////////////////////


    AsyncDatabase::AsyncDatabase(std::unique_ptr<IBackend>&& backend,
                                 std::unique_ptr<IPhotoInfoCache>&& cache,
                                 ILogger* logger):
        m_logger(logger->subLogger("AsyncDatabase")),
        m_backend(std::move(backend)),
        m_cache(std::move(cache)),
        m_executor(std::make_unique<Executor>(m_backend.get())),
        m_utils(m_cache.get(), m_backend.get(), this, m_logger.get()),
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
        exec([data](IBackend* backend)
        {
            const bool status = backend->update(data);
            assert(status);
        });
    }


    void AsyncDatabase::store(const std::vector<Photo::DataDelta>& photos)
    {
        exec([this, photos](IBackend *)
        {
             m_utils.insertPhotos(photos);
        });
    }


    void AsyncDatabase::createGroup(const Photo::Id& id, Group::Type type, const Callback<Group::Id>& callback)
    {
        exec([this, id, type, callback](IBackend* backend)
        {
            const Group::Id gid = backend->groupOperator()->addGroup(id, type);

            // mark representative as representative
            IPhotoInfo::Ptr representative = m_utils.getPhotoFor(id);
            const GroupInfo grpInfo = GroupInfo(gid, GroupInfo::Representative);
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
                IPhotoInfo::Ptr photo = m_utils.getPhotoFor(id);
                photos.push_back(photo);
            }

            callback(photos);
        });
    }


    void AsyncDatabase::listTagNames( const Callback<const std::vector<TagTypeInfo> &> & callback)
    {
        exec([callback](IBackend* backend)
        {
             const auto result = backend->listTags();

             callback(result);
        });
    }


    void AsyncDatabase::listTagValues(const TagTypeInfo& info, const Callback<const TagTypeInfo &, const std::vector<TagValue> &> & callback)
    {
        listTagValues(info, {}, callback);
    }


    void AsyncDatabase::listTagValues(const TagTypeInfo& info, const std::vector<IFilter::Ptr>& filters, const Callback<const TagTypeInfo &, const std::vector<TagValue> &> & callback)
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
                photosList.push_back(m_utils.getPhotoFor(id));

            callback(photosList);
        });
    }


    void AsyncDatabase::markStagedAsReviewed()
    {
        exec([](IBackend* backend)
        {
            backend->markStagedAsReviewed();
        });
    }


    void AsyncDatabase::execute(std::unique_ptr<ITask>&& action)
    {
        auto task = std::make_unique<CustomAction>(std::move(action));
        addTask(std::move(task));
    }


    IUtils* AsyncDatabase::utils()
    {
        return &m_utils;
    }


    IBackend* AsyncDatabase::backend()
    {
        return m_backend.get();
    }


    void AsyncDatabase::addTask(std::unique_ptr<IThreadTask>&& task)
    {
        // When task comes from from db's thread execute it immediately.
        // This simplifies some client's codes (when operating inside of execute())
        if (std::this_thread::get_id() == m_thread.get_id())
            task->execute(m_backend.get());
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

}
