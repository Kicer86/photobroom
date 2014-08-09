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

#ifndef PHOTOINFOMANAGER_H
#define PHOTOINFOMANAGER_H

#include <memory>

#include <core/photo_info.hpp>

#include "iphotoinfomanager.hpp"

namespace Database
{
    struct IDatabase;
}

class PhotoInfoManager: public IPhotoInfoManager
{
    public:
        PhotoInfoManager();
        PhotoInfoManager(const PhotoInfoManager& other) = delete;
        ~PhotoInfoManager();

        PhotoInfoManager& operator=(const PhotoInfoManager& other) = delete;

        PhotoInfo::Ptr find(const PhotoInfo::Id &) const override;
        void introduce(const PhotoInfo::Ptr &) override;

        void setDatabase(Database::IDatabase *);

    private:
        struct Data;
        std::unique_ptr<Data> m_data;
};

#endif // PHOTOINFOMANAGER_H
