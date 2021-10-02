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

#include <thread>
#include <vector>

#include "idatabase.hpp"
#include "ibackend.hpp"


struct ILogger;


namespace Database
{
    struct Executor;
    struct IThreadTask;
    struct IPhotoInfoCache;


    class Utils: public QObject, public IUtils
    {
        public:
            Utils(IPhotoInfoCache *, IBackend *, IDatabase *, ILogger *);
            ~Utils();

            IPhotoInfo::Ptr getPhotoFor(const Photo::Id & ) override;

        private:
            std::unique_ptr<ILogger> m_logger;
            IPhotoInfoCache* m_cache;
            IBackend* m_backend;
            IDatabase* m_storeKeeper;

            IPhotoInfo::Ptr constructPhotoInfo(const Photo::Data &);
            void photosModified(const std::set<Photo::Id> &);
            IPhotoInfo::Ptr findInCache(const Photo::Id &);
    };


    class AsyncDatabase: public IDatabase
    {
        public:
            AsyncDatabase(std::unique_ptr<IBackend> &&, std::unique_ptr<IPhotoInfoCache> &&, ILogger *);
            AsyncDatabase(const AsyncDatabase &) = delete;
            virtual ~AsyncDatabase();

            AsyncDatabase& operator=(const AsyncDatabase &) = delete;

            virtual void update(const Photo::DataDelta &) override;

            virtual void execute(std::unique_ptr<ITask> &&) override;

            IUtils&   utils() override;
            IBackend& backend() override;

            virtual void init(const ProjectInfo &, const Callback<const BackendStatus &> &) override;
            virtual void closeConnections() override;

        private:
            std::unique_ptr<ILogger> m_logger;
            std::unique_ptr<IBackend> m_backend;
            std::unique_ptr<IPhotoInfoCache> m_cache;
            std::unique_ptr<Executor> m_executor;
            std::thread m_thread;
            Utils m_utils;
            bool m_working;

            //store task to be executed by thread
            void addTask(std::unique_ptr<IDatabaseThread::ITask> &&);
            void stopExecutor();
    };

}

#endif // DATABASETHREAD_HPP
