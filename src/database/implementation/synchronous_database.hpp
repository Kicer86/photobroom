/*
 * Synchronous database. Implementation of IDatabase
 * which locks until result is ready.
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

#ifndef SYNCHRONOUSDATABASE_H
#define SYNCHRONOUSDATABASE_H

#include "database/idatabase.hpp"


class SynchronousDatabase: public Database::IDatabase
{
    public:

        struct IWaiter
        {
            virtual ~IWaiter() {}

            virtual void wait() = 0;
            virtual void stop() = 0;
        };


        SynchronousDatabase();
        SynchronousDatabase(const SynchronousDatabase &) = delete;
        ~SynchronousDatabase();
        SynchronousDatabase& operator=(const SynchronousDatabase &) = delete;

        void setDatabase(Database::IDatabase *);
        void setWaiter(IWaiter *);

        virtual void closeConnections() override;
        virtual void getAllPhotos(const Database::Task&) override;
        virtual void getPhoto(const Database::Task&, const PhotoInfo::Id&) override;
        virtual void getPhotos(const Database::Task&, const std::deque< Database::IFilter::Ptr >&) override;
        virtual bool init(const Database::Task&, const std::string&) override;
        virtual void listTags(const Database::Task&) override;
        virtual void listTagValues(const Database::Task&, const TagNameInfo&) override;
        virtual void listTagValues(const Database::Task&, const TagNameInfo&, const std::deque< Database::IFilter::Ptr >&) override;
        virtual Database::ADatabaseSignals* notifier() override;
        virtual Database::Task prepareTask(Database::IDatabaseClient*) override;
        virtual void store(const Database::Task&, const PhotoInfo::Ptr&) override;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

#endif // SYNCHRONOUSDATABASE_H
