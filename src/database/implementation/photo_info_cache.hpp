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

#ifndef PHOTOINFOMANAGER_H
#define PHOTOINFOMANAGER_H

#include <memory>
#include <unordered_map>

#include <database/iphoto_info.hpp>

#include "iphoto_info_cache.hpp"

namespace Database
{
    struct IDatabase;
}

class PhotoInfoCache: public Database::IPhotoInfoCache
{
    public:
        PhotoInfoCache();
        PhotoInfoCache(const PhotoInfoCache& other) = delete;
        ~PhotoInfoCache();

        PhotoInfoCache& operator=(const PhotoInfoCache& other) = delete;

        IPhotoInfo::Ptr find(const Photo::Id &) const override;
        void introduce(const IPhotoInfo::Ptr &) override;
        void forget(const Photo::Id &) override;

    private:
        // Cache for all introduced photos.
        // Photos are kept as weak_ptrs so cache won't increase ref count and photo
        // can be released when there no clients.
        // All introduced photos need (?) to be remembered here, so where won't be more than one
        // independent IPhotoInfos which could damage DB when modified in parallel.
        std::unordered_map<Photo::Id, std::weak_ptr<IPhotoInfo>, Photo::IdHash> m_photo_cache;
};

#endif // PHOTOINFOMANAGER_H
