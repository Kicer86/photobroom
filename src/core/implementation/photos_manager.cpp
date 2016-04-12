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
#include <QFileInfo>
#include <QImage>


struct PhotosManager::Data
{
    Data(): m_mutex(), m_cache(16), m_basePath() {}

    std::mutex m_mutex;
    QCache<QString, QByteArray> m_cache;
    QString m_basePath;
};


PhotosManager::PhotosManager(): IPhotosManager(), m_data(new Data)
{

}


PhotosManager::~PhotosManager()
{

}


void PhotosManager::setBasePath(const QString& path)
{
    std::lock_guard<std::mutex> lock(m_data->m_mutex);
    m_data->m_basePath = path;
    m_data->m_cache.clear();
}


QByteArray PhotosManager::getPhoto(const IPhotoInfo::Ptr& photoInfo)
{
    return getPhoto(photoInfo->getPath());
}


QByteArray PhotosManager::getPhoto(const QString& path)
{
    std::lock_guard<std::mutex> lock(m_data->m_mutex);
    assert(m_data->m_basePath.isEmpty() == false);

    const QFileInfo pathInfo(path);
    QString absolutePath = pathInfo.isAbsolute()? path: QDir(m_data->m_basePath).filePath(path);
    absolutePath = QDir().cleanPath(absolutePath);

    QByteArray* result = m_data->m_cache.object(path);

    if (result == nullptr)
    {
        QFile photo(absolutePath);
        photo.open(QIODevice::ReadOnly);

        result = new QByteArray(photo.readAll());

        m_data->m_cache.insert(absolutePath, result);
    }

    return *result;
}


QImage PhotosManager::getThumbnail(const QString& path)
{
    QByteArray raw = getPhoto(path);

    QImage image;
    image.loadFromData(raw);

    const int ih = image.height();

    // TODO: remove constants, use settings?
    const bool needs_resize = ih > 120;

    // scale image so its height == 120
    const QImage scaled = needs_resize?
                          image.scaledToHeight(120, Qt::SmoothTransformation):
                          image;

    return scaled;
}
