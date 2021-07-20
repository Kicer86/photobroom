/*
 * Photo Broom - photos management tool.
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


uint qHash(const IThumbnailsCache::ThumbnailParameters& params)
{
    const QSize& size = std::get<0>(params);

    return qHash(size.width()) ^ qHash(size.height());
}

uint qHash(const std::tuple<Photo::Id, IThumbnailsCache::ThumbnailParameters>& key)
{
    const Photo::Id& id = std::get<0>(key);
    const IThumbnailsCache::ThumbnailParameters& params = std::get<1>(key);

    return qHash(id.value()) ^ qHash(params);
}


ThumbnailsCache::ThumbnailsCache(): m_cache(1024)
{
}


std::optional<QImage> ThumbnailsCache::find(const Photo::Id& id, const ThumbnailParameters& params)
{
    std::optional<QImage> result;

    QImage* img = m_cache.lock()->object(std::tie(id, params));

    if (img)
        result = *img;

    return result;
}


void ThumbnailsCache::store(const Photo::Id& id, const ThumbnailParameters& params, const QImage& img)
{
    QImage* copy = new QImage(img);
    m_cache.lock()->insert(std::tie(id, params), copy);
}
