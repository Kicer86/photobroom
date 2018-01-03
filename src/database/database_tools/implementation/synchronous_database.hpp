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

#include <database/iphoto_info.hpp>
#include <database/filter.hpp>

namespace Database
{
    struct IDatabase;
}

class SynchronousDatabase
{
    public:
        SynchronousDatabase();
        SynchronousDatabase(const SynchronousDatabase &) = delete;
        ~SynchronousDatabase();

        SynchronousDatabase& operator=(const SynchronousDatabase &) = delete;

        void set(Database::IDatabase *);

        // functionality of IDatabase:
        const IPhotoInfo::List getPhotos(const std::vector<Database::IFilter::Ptr> &);  //list all photos matching filter

    private:
        Database::IDatabase* m_database;
};

#endif // SYNCHRONOUSDATABASE_H
