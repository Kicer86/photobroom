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

#ifndef PHOTO_INFO_STOREKEEPER_HPP
#define PHOTO_INFO_STOREKEEPER_HPP

#include <memory>

#include <database/implementation/photo_info.hpp>
#include <ibackend.hpp>

#include "iphoto_info_cache.hpp"
#include "iphoto_info_storekeeper.hpp"

namespace Database
{
    struct IDatabase;
}

class PhotoInfo;

class PhotoInfoStorekeeper: public PhotoInfo::IObserver, public IPhotoInfoStorekeeper
{
    public:
        PhotoInfoStorekeeper();
        PhotoInfoStorekeeper(const PhotoInfoStorekeeper &) = delete;
        ~PhotoInfoStorekeeper();

        PhotoInfoStorekeeper& operator=(const PhotoInfoStorekeeper &) = delete;

        void photoInfoConstructed(PhotoInfo *);
        void setDatabase(Database::IDatabase *);
        void setCache(Database::IPhotoInfoCache *);

    private:
        struct Data;
        std::unique_ptr<Data> m_data;

        virtual void photoUpdated(IPhotoInfo *, PhotoInfo::ChangeReason) override;

        void storeTags(const Photo::Id &, const Tag::TagsList &) override;
};

#endif // PHOTO_INFO_STOREKEEPER_HPP
