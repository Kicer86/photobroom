/*
 * PhotoInfoCache which purpose is to cache IPhotoInfos
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

#include "photo_info_cache.hpp"

#include <database/iphoto_info.hpp>

#include <unordered_map>

#include <idatabase.hpp>


struct PhotoInfoCache::Data
{
    Data(): m_photo_cache() {}
    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    ~Data() {}

    std::unordered_map<Photo::Id, std::weak_ptr<IPhotoInfo>, Photo::IdHash> m_photo_cache;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

PhotoInfoCache::PhotoInfoCache(): m_data(new Data)
{
}


PhotoInfoCache::~PhotoInfoCache()
{

}


IPhotoInfo::Ptr PhotoInfoCache::find(const Photo::Id& id) const
{
    IPhotoInfo::Ptr result;
    auto it = m_data->m_photo_cache.find(id);

    if (it != m_data->m_photo_cache.end())
        result = it->second.lock();

    return result;
}


void PhotoInfoCache::introduce(const IPhotoInfo::Ptr& ptr)
{
    const auto id = ptr->getID();
    m_data->m_photo_cache[id] = ptr;
}


void PhotoInfoCache::forget(const Photo::Id& id)
{
    auto it = m_data->m_photo_cache.find(id);

    assert(it != m_data->m_photo_cache.end() );
    m_data->m_photo_cache.erase(it);
}
