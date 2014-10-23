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

#include "synchronous_database.hpp"

SynchronousDatabase::SynchronousDatabase(): m_database(nullptr)
{

}


SynchronousDatabase::~SynchronousDatabase()
{

}


void SynchronousDatabase::set(Database::IDatabase* database)
{
    m_database = database;
}


const Database::QueryList& SynchronousDatabase::getPhotos(const std::deque< Database::IFilter::Ptr >& filters)
{

}


void SynchronousDatabase::got_getAllPhotos(const Database::Task&, const Database::QueryList&)
{

}


void SynchronousDatabase::got_getPhoto(const Database::Task&, const IPhotoInfo::Ptr&)
{

}


void SynchronousDatabase::got_getPhotos(const Database::Task&, const Database::QueryList&)
{

}


void SynchronousDatabase::got_listTags(const Database::Task&, const std::vector< TagNameInfo >&)
{

}


void SynchronousDatabase::got_listTagValues(const Database::Task&, const std::deque< TagValueInfo >&)
{

}


void SynchronousDatabase::got_storeStatus(const Database::Task&)
{

}
