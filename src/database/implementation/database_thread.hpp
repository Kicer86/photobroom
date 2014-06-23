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

namespace Database
{

    class DatabaseThread: public IDatabase
    {
        public:
            DatabaseThread(std::unique_ptr<IBackend> &&);
            DatabaseThread(const DatabaseThread &) = delete;
            virtual ~DatabaseThread();

            DatabaseThread& operator=(const DatabaseThread &) = delete;

            virtual Task prepareTask(IDatabaseClient *) override;

            virtual void store(const Task &, const PhotoInfo::Ptr &) override;

            virtual void getAllPhotos(const Task &) override;
            virtual void getPhoto(const Task &, const PhotoInfo::Id &) override;
            virtual void getPhotos(const Task &, const std::deque<IFilter::Ptr> &) override;
            virtual void listTags(const Task &) override;
            virtual void listTagValues(const Task &, const TagNameInfo &) override;
            virtual void listTagValues(const Task &, const TagNameInfo &, const std::deque<IFilter::Ptr> &) override;

            virtual bool init(const char*) override;
            virtual void closeConnections() override;

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;
    };

}

#endif // DATABASETHREAD_H
