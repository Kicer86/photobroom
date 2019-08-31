/*
 * Simple memory cache for thumbnails.
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "thumbnails_cache.hpp"

uint qHash(const std::tuple<QString, int>& key)
{
    const QString& path = std::get<0>(key);
    const int height = std::get<1>(key);

    return qHash(path) ^ qHash(height);
}


std::optional<QImage> ThumbnailsCache::find(const QString& path, int height)
{
    std::optional<QImage> result;

    QImage* img = m_cache[std::tie(path, height)];

    if (img)
        result = *img;

    return result;
}


void ThumbnailsCache::store(const QString& path, int height, const QImage& img)
{
    QImage* copy = new QImage(img);
    m_cache.insert(std::tie(path, height), copy);
}
