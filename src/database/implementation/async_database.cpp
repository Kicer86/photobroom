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

#include <core/logger_factory.hpp>
#include <core/thread_utils.hpp>
#include <core/ts_queue.hpp>

#include "ibackend.hpp"
#include "igroup_operator.hpp"
#include "iphoto_operator.hpp"
#include "photo_data.hpp"
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
                std::optional< std::unique_ptr<IDatabase::ITask> > taskOpt = m_tasks.pop();

                if (taskOpt)
                {
                    QElapsedTimer timer;
                    timer.start();

                    const auto& task = *taskOpt;

                    task->run(m_backend);

                    const qint64 elapsed = timer.elapsed();
                    const QString message = QString("task '%2' took %1ms")
                        .arg(elapsed)
                        .arg(QString::fromStdString(task->name()));

                    if (elapsed > 100)
                        m_logger->warning(message);
                    else
                        m_logger->trace(message);
                }
                else
                    break;
            }
        }

        void stop()
        {
            m_tasks.stop();
        }


        void addTask(std::unique_ptr<IDatabase::ITask>&& task)
        {
            m_tasks.push(std::move(task));
        }

        private:
            ol::TS_Queue<std::unique_ptr<IDatabase::ITask>> m_tasks;
            Database::IBackend& m_backend;
            std::unique_ptr<ILogger> m_logger;
    };

    struct DbCloseTask final: IDatabase::ITask
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


    AsyncDatabase::Client::Client(AsyncDatabase& _db, QStringView name)
        : m_name(name.toString())
        , m_db(_db)
    {

    }


    AsyncDatabase::Client::~Client()
    {
        m_db.remove(this);
    }


    IDatabase& AsyncDatabase::Client::db()
    {
        return m_db;
    }


    void AsyncDatabase::Client::onClose(const std::function<void()>& onClose_)
    {
        m_onClose = onClose_;
    }


    void AsyncDatabase::Client::callOnClose() const
    {
        if (m_onClose)
            m_onClose();
    }


    const QString& AsyncDatabase::Client::name() const
    {
        return m_name;
    }

    ///////////////////////////////////////////////////////////////////////////


    AsyncDatabase::AsyncDatabase(std::unique_ptr<IBackend>&& backend, ILogger* logger)
        : m_logger(logger->subLogger("AsyncDatabase"))
        , m_backend(std::move(backend))
        , m_executor(std::make_unique<Executor>(*m_backend.get(), m_logger.get()))
        , m_working(true)
    {
        m_thread = std::thread(&Executor::begin, m_executor.get());
    }


    AsyncDatabase::~AsyncDatabase()
    {
        //terminate thread
        close();
    }


    void AsyncDatabase::init(const ProjectInfo& prjInfo, const Callback<const BackendStatus &>& callback)
    {
        exec([prjInfo, callback](IBackend& backend)
        {
             const Database::BackendStatus status = backend.init(prjInfo);

             callback(status);
        });
    }


    void AsyncDatabase::execute(std::unique_ptr<IDatabase::ITask>&& task)
    {
        addTask(std::move(task));
    }


    IBackend& AsyncDatabase::backend()
    {
        return *m_backend.get();
    }


    void AsyncDatabase::close()
    {
        // close clients
        m_acceptClients = false;

        sendOnCloseNotification();
        waitForClients();

        // finish tasks
        stopExecutor();
    }


    std::unique_ptr<IClient> AsyncDatabase::attach(QStringView name, std::function<void()> onClose)
    {
        if (m_acceptClients)
        {
            auto observer = std::make_unique<Client>(*this, name);
            observer->onClose(onClose);

            std::lock_guard _(m_clientsMutex);
            m_clients.insert(observer.get());

            return observer;
        }
        else
            return {};
    }


    void AsyncDatabase::addTask(std::unique_ptr<IDatabase::ITask>&& task)
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


    void AsyncDatabase::sendOnCloseNotification()
    {
        assert(m_acceptClients == false);

        std::unique_lock lk(m_clientsMutex);

        // client may be removed during the loop below, so work on a copy
        const auto clients = m_clients;

        for(auto& client: clients)
        {
            m_logger->debug("Sending close notification to " + client->name());
            client->callOnClose();
        }
    }


    void AsyncDatabase::waitForClients()
    {
        std::unique_lock lk(m_clientsMutex);

        m_logger->debug("Active clients:");
        for(auto& client: m_clients)
            m_logger->debug(client->name());

        m_clientsChangeCV.wait(lk, [this]()
        {
            return m_clients.empty();
        });
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


    void AsyncDatabase::remove(Client* c)
    {
        std::lock_guard _(m_clientsMutex);
        m_clients.erase(c);
        m_clientsChangeCV.notify_all();
    }

}
