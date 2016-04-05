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

#ifndef DATABASETHREAD_H
#define DATABASETHREAD_H

#include <deque>

#include "idatabase.hpp"
#include "ibackend.hpp"

namespace Database
{

    class DatabaseThread: public IDatabase
    {
        public:
            DatabaseThread(std::unique_ptr<IBackend> &&);
            DatabaseThread(const DatabaseThread &) = delete;
            virtual ~DatabaseThread();

            DatabaseThread& operator=(const DatabaseThread &) = delete;

            void set(std::unique_ptr<IPhotoInfoCache> &&);

            virtual ADatabaseSignals* notifier() override;

            virtual void exec(std::unique_ptr<AStorePhotoTask> &&, const QString &) override;
            virtual void exec(std::unique_ptr<AStorePhotoTask> &&, const IPhotoInfo::Ptr &) override;
            virtual void exec(std::unique_ptr<AStoreTagTask>&&, const TagNameInfo&) override;

            virtual void exec(std::unique_ptr<AListTagsTask> &&) override;
            virtual void exec(std::unique_ptr<AGetPhotoTask> &&, const Photo::Id &) override;
            virtual void exec(std::unique_ptr<AGetPhotosTask> &&, const std::deque<IFilter::Ptr> &) override;
            virtual void exec(std::unique_ptr<AGetPhotosTask> &&) override;
            virtual void exec(std::unique_ptr<AListTagValuesTask> &&, const TagNameInfo &) override;
            virtual void exec(std::unique_ptr<AListTagValuesTask> &&, const TagNameInfo &, const std::deque<IFilter::Ptr> &) override;
            virtual void exec(std::unique_ptr<AGetPhotosCount> &&, const std::deque< IFilter::Ptr >&) override;

            virtual void exec(std::unique_ptr<ADropPhotosTask> &&, const std::deque< IFilter::Ptr >&) override;

            virtual void exec(std::unique_ptr<AInitTask> &&, const ProjectInfo &) override;
            virtual void closeConnections() override;

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;
    };

}

#endif // DATABASETHREAD_H
