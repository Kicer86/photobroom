/*
 * Photo Broom - photos management tool.
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
#include <QElapsedTimer>

#include <core/down_cast.hpp>
#include <core/logger_factory.hpp>
#include <core/thread_utils.hpp>
#include <core/ts_queue.hpp>

#include "ibackend.hpp"
#include "igroup_operator.hpp"
#include "iphoto_info_cache.hpp"
#include "iphoto_operator.hpp"
#include "photo_data.hpp"
#include "photo_info.hpp"
#include "project_info.hpp"


namespace Database
{
    struct Executor
    {
        Executor(Database::IBackend& backend, ILogger* logger):
            m_tasks(1024),
            m_backend(backend),
            m_logger(logger->subLogger("Executor"))
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
                std::optional< std::unique_ptr<IDatabaseThread::ITask> > taskOpt = m_tasks.pop();

                if (taskOpt)
                {
                    QElapsedTimer timer;
                    timer.start();

                    const auto& task = *taskOpt;

                    task->run(m_backend);

                    const qint64 elapsed = timer.elapsed();

                    if (elapsed > 100)
                    {
                        const QString message = QString("DB task '%2' took %1ms")
                            .arg(elapsed)
                            .arg(QString::fromStdString(task->name()));

                        m_logger->warning(message);
                    }
                }
                else
                    break;
            }
        }

        void stop()
        {
            m_tasks.stop();
        }


        void addTask(std::unique_ptr<IDatabaseThread::ITask>&& task)
        {
            m_tasks.push(std::move(task));
        }

        private:
            ol::TS_Queue<std::unique_ptr<IDatabaseThread::ITask>> m_tasks;
            Database::IBackend& m_backend;
            std::unique_ptr<ILogger> m_logger;
    };

    struct DbCloseTask final: IDatabaseThread::ITask
    {
        DbCloseTask() = default;

        void run(IBackend& backend) override
        {
            backend.closeConnections();
        }

        std::string name() override
        {
            return "DB close";
        }
    };


    ///////////////////////////////////////////////////////////////////////////


    Utils::Utils(IPhotoInfoCache* cache, IBackend* backend, IDatabase* keeper, ILogger* logger):
        m_logger(logger->subLogger("Utils")),
        m_cache(cache),
        m_backend(backend),
        m_storeKeeper(keeper)
    {
        connect(backend, &IBackend::photosModified, this, &Utils::photosModified, Qt::DirectConnection);
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


    IPhotoInfo::Ptr Utils::constructPhotoInfo(const Photo::Data& data)
    {
        auto photoInfo = std::make_shared<PhotoInfo>(data, m_storeKeeper);

        m_cache->introduce(photoInfo);

        const QString insert_msg = QString("Adding photo with id %1 to cache").arg(data.id);
        m_logger->debug(insert_msg);

        return photoInfo;
    }


    void Utils::photosModified(const std::set<Photo::Id>& ids)
    {
        for (const Photo::Id& id: ids)
        {
            auto photoInfo = findInCache(id);

            if (photoInfo.get() != nullptr)
            {
                const Photo::Data photoData = m_backend->getPhoto(id);

                photoInfo->setData(photoData);
            }
        }
    }


    IPhotoInfo::Ptr Utils::findInCache(const Photo::Id& id)
    {
        const QString search_msg = QString("Looking for photo with id %1 in cache").arg(id);
        m_logger->trace(search_msg);

        auto photoInfo = m_cache->find(id);

        if (photoInfo.get() == nullptr)
        {
            const QString result_msg = QString("Photo with id %1 not found in cache").arg(id);
            m_logger->trace(result_msg);
        }
        else
        {
            const QString result_msg = QString("Photo with id %1 found in cache").arg(id);
            m_logger->trace(result_msg);
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
        m_executor(std::make_unique<Executor>(*m_backend.get(), m_logger.get())),
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
        exec([prjInfo, callback](IBackend& backend)
        {
             const Database::BackendStatus status = backend.init(prjInfo);

             callback(status);
        });
    }


    void AsyncDatabase::update(const Photo::DataDelta& data)
    {
        exec([data](IBackend& backend)
        {
            const bool status = backend.update( {data} );
            assert(status);
        });
    }


    void AsyncDatabase::execute(std::unique_ptr<IDatabaseThread::ITask>&& task)
    {
        addTask(std::move(task));
    }


    IUtils& AsyncDatabase::utils()
    {
        return m_utils;
    }


    IBackend& AsyncDatabase::backend()
    {
        return *m_backend.get();
    }


    void AsyncDatabase::addTask(std::unique_ptr<IDatabaseThread::ITask>&& task)
    {
        // When task comes from from db's thread execute it immediately.
        // This simplifies some client's codes (when operating inside of execute())
        if (std::this_thread::get_id() == m_thread.get_id())
            task->run(*m_backend.get());
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
