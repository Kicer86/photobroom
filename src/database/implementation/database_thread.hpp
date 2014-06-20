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
            DatabaseThread(IBackend *);
            DatabaseThread(const DatabaseThread &) = delete;
            virtual ~DatabaseThread();

            DatabaseThread& operator=(const DatabaseThread &) = delete;

            virtual bool store(const PhotoInfo::Ptr&) override;

            virtual void getAllPhotos(IDatabaseClient*);
            virtual void getPhoto(const PhotoInfo::Id&, IDatabaseClient*);
            virtual void getPhotos(const std::deque< IFilter::Ptr >&, IDatabaseClient*);
            virtual void listTags(IDatabaseClient*);
            virtual void listTagValues(const TagNameInfo&, IDatabaseClient*);
            virtual void listTagValues(const TagNameInfo&, const std::deque< IFilter::Ptr >&, IDatabaseClient*);

            virtual bool init(const char*) override;

            virtual void closeConnections() override;

        private:
            IBackend* m_backend;
    };

}

#endif // DATABASETHREAD_H
