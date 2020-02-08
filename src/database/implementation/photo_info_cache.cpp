/*
 * Photo Broom - photos management tool.
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

#include <core/ilogger.hpp>
#include <core/ilogger_factory.hpp>
#include <database/iphoto_info.hpp>

#include <idatabase.hpp>


///////////////////////////////////////////////////////////////////////////////////////////////////

PhotoInfoCache::PhotoInfoCache(ILogger* l):
    m_logger(l->subLogger("PhotoInfoCache"))
{
}


PhotoInfoCache::~PhotoInfoCache()
{

}


IPhotoInfo::Ptr PhotoInfoCache::find(const Photo::Id& id) const
{
    IPhotoInfo::Ptr result;
    auto it = m_photo_cache.find(id);

    if (it != m_photo_cache.end())
    {
        result = it->second.lock();

        if (result.get() == nullptr)
        {
            const QString msg = QString("Photo with id %1 was recently used but has no clients at this moment.").arg(id);
            m_logger->debug(msg);
        }
    }

    return result;
}


void PhotoInfoCache::introduce(const IPhotoInfo::Ptr& ptr)
{
    const auto id = ptr->getID();

    m_photo_cache[id] = ptr;
}


void PhotoInfoCache::forget(const Photo::Id& id)
{
    auto it = m_photo_cache.find(id);

    assert(it != m_photo_cache.end() );

    m_photo_cache.erase(it);
}
