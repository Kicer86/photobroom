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
#include <QDir>
#include <QFile>
#include <QImage>


struct PhotosManager::Data
{
    Data():
        m_mutex(),
        m_thumbnails_mutex(),
        m_cache(64),
        m_thumbnails(256)
    {

    }

    std::mutex m_mutex;
    std::mutex m_thumbnails_mutex;
    QCache<QString, QByteArray> m_cache;
    QCache< std::pair<QString, int>, QImage> m_thumbnails;
};


uint qHash(const std::pair<QString, int>& key, uint seed = 0)
{
    return qHash(key.first) ^ qHash(key.second) ^ seed;
}


PhotosManager::PhotosManager(): IPhotosManager(), m_data(new Data)
{

}


PhotosManager::~PhotosManager()
{

}


QByteArray PhotosManager::getPhoto(const QString& path) const
{
    std::lock_guard<std::mutex> lock(m_data->m_mutex);

    const QString cleanPath = QDir().cleanPath(path);

    QByteArray* result = m_data->m_cache.object(cleanPath);

    if (result == nullptr)
    {
        QFile photo(cleanPath);
        photo.open(QIODevice::ReadOnly);

        result = new QByteArray(photo.readAll());

        m_data->m_cache.insert(cleanPath, result);
    }

    return *result;
}
