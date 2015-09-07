/*
 * Photos Manager. Loads and caches raw photos data.
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

#ifndef PHOTOSMANAGER_H
#define PHOTOSMANAGER_H

#include <memory>

#include <QByteArray>

#include <database/iphoto_info.hpp>

#include "core_export.h"



class QString;

class CORE_EXPORT PhotosManager
{
    public:
        PhotosManager(const PhotosManager &) = delete;
        ~PhotosManager();

        static PhotosManager* instance();

        PhotosManager& operator=(const PhotosManager &) = delete;

        QByteArray getPhoto(const IPhotoInfo::Ptr &);
        QByteArray getPhoto(const QString& path);

    private:
        PhotosManager();
        struct Data;
        std::unique_ptr<Data> m_data;
};

#endif // PHOTOSMANAGER_H
