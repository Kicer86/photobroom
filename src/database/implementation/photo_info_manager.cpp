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

#include "photo_info_manager.hpp"

#include <database/iphoto_info.hpp>

#include <unordered_map>

#include <idatabase.hpp>


struct PhotoInfoManager::Data: Database::IDatabaseClient
{
    Data(): m_photo_cache(), m_database(nullptr) {}
    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    ~Data() {}

    std::unordered_map<IPhotoInfo::Id, std::weak_ptr<IPhotoInfo>, PhotoInfoIdHash> m_photo_cache;
    Database::IDatabase* m_database;

    virtual void got_getAllPhotos(const Database::Task &, const Database::QueryList &) override {}
    virtual void got_getPhoto(const Database::Task &, const IPhotoInfo::Ptr &) override {}
    virtual void got_getPhotos(const Database::Task &, const Database::QueryList &) override {}
    virtual void got_listTags(const Database::Task &, const std::vector<TagNameInfo> &) override {}
    virtual void got_listTagValues(const Database::Task &, const std::deque<TagValueInfo> &) override {}
    virtual void got_storeStatus(const Database::Task &) override {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

PhotoInfoManager::PhotoInfoManager(): m_data(new Data)
{
}


PhotoInfoManager::~PhotoInfoManager()
{

}


IPhotoInfo::Ptr PhotoInfoManager::find(const IPhotoInfo::Id& id) const
{
    IPhotoInfo::Ptr result;
    auto it = m_data->m_photo_cache.find(id);

    if (it != m_data->m_photo_cache.end())
        result = it->second.lock();

    return result;
}


void PhotoInfoManager::introduce(const IPhotoInfo::Ptr& ptr)
{
    const auto id = ptr->getID();
    m_data->m_photo_cache[id] = ptr;

    ptr->registerObserver(this);
}


void PhotoInfoManager::setDatabase(Database::IDatabase* database)
{
    m_data->m_database = database;
}


//TODO: those conditions there doesn't look nice...
//is there nicer way for direct access to IPhotoInfo::Ptr?
void PhotoInfoManager::photoUpdated(IPhotoInfo* photoInfo)
{
    //find photo in cache
    IPhotoInfo::Id id = photoInfo->getID();
    IPhotoInfo::Ptr ptr = find(id);

    //we should be aware of all exisitng photo info
    assert(ptr.get() != nullptr);

    //when found update changed photo in database
    if (ptr.get() != nullptr && ptr->isFullyInitialized())
    {
        auto task = m_data->m_database->prepareTask(m_data.get());
        m_data->m_database->update(task, ptr);
    }
}
