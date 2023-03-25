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

#ifndef DATABASETHREAD_HPP
#define DATABASETHREAD_HPP

#include <condition_variable>
#include <thread>
#include <vector>

#include "idatabase.hpp"
#include "ibackend.hpp"


struct ILogger;


namespace Database
{
    struct Executor;
    struct IThreadTask;


    class AsyncDatabase: public IDatabase
    {
        public:
            AsyncDatabase(std::unique_ptr<IBackend> &&, ILogger *);
            AsyncDatabase(const AsyncDatabase &) = delete;
            virtual ~AsyncDatabase();

            AsyncDatabase& operator=(const AsyncDatabase &) = delete;

            virtual void execute(std::unique_ptr<ITask> &&) override;

            IBackend& backend() override;

            virtual void init(const ProjectInfo &, const Callback<const BackendStatus &> &) override;
            virtual void closeConnections() override;

            std::unique_ptr<IClient> attach(const QString &) override;

        private:
            struct Client: IClient
            {
                Client(AsyncDatabase *);
                ~Client() override;

                AsyncDatabase* db;
            };

            friend struct Client;

            std::set<Client *> m_clients;
            std::mutex m_clientsMutex;
            std::unique_ptr<ILogger> m_logger;
            std::unique_ptr<IBackend> m_backend;
            std::unique_ptr<Executor> m_executor;
            std::condition_variable m_clientsChangeCV;
            std::thread m_thread;
            bool m_working;

            void addTask(std::unique_ptr<IDatabaseThread::ITask> &&);
            void waitForClients();
            void stopExecutor();
            void remove(Client *);
    };

}

#endif // DATABASETHREAD_HPP
