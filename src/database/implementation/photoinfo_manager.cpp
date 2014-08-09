/*
 * PhotoInfo Manager
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

#include "photoinfo_manager.hpp"

#include <core/photo_info.hpp>

#include <unordered_map>

#include "photo_info_updater.hpp"
#include <idatabase.hpp>

struct PhotoInfoIdHash
{
    std::size_t operator()(const PhotoInfo::Id& key) const
    {
        return std::hash<PhotoInfo::Id::type>()(key.value());
    }
};


struct PhotoInfoManager::Data
{
    Data(): m_photo_cache(), m_database(nullptr) {}
    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    ~Data() {}

    std::unordered_map<PhotoInfo::Id, std::weak_ptr<PhotoInfo>, PhotoInfoIdHash> m_photo_cache;
    Database::IDatabase* m_database;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

PhotoInfoManager::PhotoInfoManager(): m_data(new Data)
{
}


PhotoInfoManager::~PhotoInfoManager()
{

}


PhotoInfo::Ptr PhotoInfoManager::find(const PhotoInfo::Id& id) const
{
    PhotoInfo::Ptr result;
    auto it = m_data->m_photo_cache.find(id);

    if (it != m_data->m_photo_cache.end())
        result = it->second.lock();

    return result;
}


void PhotoInfoManager::introduce(const PhotoInfo::Ptr& ptr)
{
    const auto id = ptr->getID();
    m_data->m_photo_cache[id] = ptr;

    ptr->registerObserver(this);

    if (ptr->isFullyInitialized() == false)
    {
        if (ptr->isHashLoaded() == false)
            PhotoInfoUpdater::updateHash(ptr);

        if (ptr->isThumbnailLoaded() == false)
            PhotoInfoUpdater::updateThumbnail(ptr);

        if (ptr->isExifDataLoaded() == false)
            PhotoInfoUpdater::updateTags(ptr);
    }
}


void PhotoInfoManager::setDatabase(Database::IDatabase* database)
{
    m_data->m_database = database;
}


//TODO: those conditions there doesn't look nice...
//is there nicer way for direct access to PhotoInfo::Ptr?
void PhotoInfoManager::photoUpdated(PhotoInfo* photoInfo)
{
    //find photo in cache
    PhotoInfo::Id id = photoInfo->getID();
    PhotoInfo::Ptr ptr = find(id);

    //we should be aware of all exisitng photo info
    assert(ptr.get() != nullptr);

    //when found update changed photo in database
    if (ptr.get() != nullptr)
    {
        auto task = m_data->m_database->prepareTask(nullptr);
        m_data->m_database->update(task, ptr);
    }
}
