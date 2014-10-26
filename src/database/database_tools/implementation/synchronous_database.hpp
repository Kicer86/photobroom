/*
 * Decorator for IDatabase which makes it synchronous
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

#ifndef SYNCHRONOUSDATABASE_HPP
#define SYNCHRONOUSDATABASE_HPP

#include <database/idatabase.hpp>


class SynchronousDatabase: Database::IDatabaseClient
{
    public:
        SynchronousDatabase();
        SynchronousDatabase(const SynchronousDatabase &) = delete;
        ~SynchronousDatabase();

        SynchronousDatabase& operator=(const SynchronousDatabase &) = delete;

        void set(Database::IDatabase *);

        // functionality of IDatabase:
        const IPhotoInfo::List& getPhotos(const std::deque<Database::IFilter::Ptr> &);  //list all photos matching filter

    private:
        Database::IDatabase* m_database;

        virtual void got_getAllPhotos(const Database::Task&, const IPhotoInfo::List &) override;
        virtual void got_getPhoto(const Database::Task&, const IPhotoInfo::Ptr &) override;
        virtual void got_getPhotos(const Database::Task&, const IPhotoInfo::List &) override;
        virtual void got_listTags(const Database::Task&, const std::deque< TagNameInfo >&) override;
        virtual void got_listTagValues(const Database::Task&, const std::deque< TagValueInfo >&) override;
        virtual void got_storeStatus(const Database::Task&) override;
};

#endif // SYNCHRONOUSDATABASE_H
