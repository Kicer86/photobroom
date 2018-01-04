/*
 * PhotoInfoStorekeeper - stores changed photos in database
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

#include "photo_info_storekeeper.hpp"

#include <idatabase.hpp>


struct PhotoInfoStorekeeper::Data
{
    Data(): m_cache(nullptr), m_database(nullptr) {}
    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    ~Data() {}

    Database::IPhotoInfoCache* m_cache;
    Database::IDatabase* m_database;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

PhotoInfoStorekeeper::PhotoInfoStorekeeper(): m_data(new Data)
{
}


PhotoInfoStorekeeper::~PhotoInfoStorekeeper()
{

}


void PhotoInfoStorekeeper::setDatabase(Database::IDatabase* database)
{
    m_data->m_database = database;
}


void PhotoInfoStorekeeper::setCache(Database::IPhotoInfoCache* cache)
{
    m_data->m_cache = cache;
}


void PhotoInfoStorekeeper::photoUpdated(IPhotoInfo* photoInfo, PhotoInfo::ChangeReason)
{
    //find photo in cache (alternative is to use std::enable_shared_from_this but is it worth it?)
    const Photo::Id id = photoInfo->getID();
    IPhotoInfo::Ptr ptr = m_data->m_cache->find(id);

    //we should be aware of all exisitng photo info
    assert(ptr.get() != nullptr || photoInfo->isValid() == false);

    //if found, update changed photo in database (but only if fully loaded)
    if (ptr.get() != nullptr && ptr->isFullyInitialized())
        m_data->m_database->update(ptr);
}


void PhotoInfoStorekeeper::storeTags(const Photo::Id&, const Tag::TagsList&)
{

}


void PhotoInfoStorekeeper::photoInfoConstructed(PhotoInfo* photoInfo)
{
    photoInfo->registerObserver(this);
}
