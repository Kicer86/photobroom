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

#include "photos_manager.hpp"

#include <mutex>

#include <QCache>
#include <QFile>



struct PhotosManager::Data
{
    Data(): m_mutex(), m_cache(16) {}

    std::mutex m_mutex;
    QCache<QString, QByteArray> m_cache;
};


PhotosManager::PhotosManager(): m_data(new Data)
{

}


PhotosManager::~PhotosManager()
{

}


PhotosManager* PhotosManager::instance()
{
    static PhotosManager _instance;

    return &_instance;
}


QByteArray PhotosManager::getPhoto(const IPhotoInfo::Ptr& photoInfo)
{
    return getPhoto(photoInfo->getPath());
}


QByteArray PhotosManager::getPhoto(const QString& path)
{
    std::unique_lock<std::mutex> lock(m_data->m_mutex);

    QByteArray* result = m_data->m_cache.object(path);

    if (result == nullptr)
    {
        QFile photo(path);
        photo.open(QIODevice::ReadOnly);

        result = new QByteArray(photo.readAll());

        m_data->m_cache.insert(path, result);
    }

    return *result;
}
