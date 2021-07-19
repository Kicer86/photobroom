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

#ifndef THUMBNAILS_CACHE_HPP
#define THUMBNAILS_CACHE_HPP

#include <QCache>
#include <OpenLibrary/putils/ts_resource.hpp>

#include "ithumbnails_cache.hpp"


class ThumbnailsCache: public IThumbnailsCache
{
    public:
        ThumbnailsCache();

        std::optional<QImage> find(const Photo::Id &, const ThumbnailParameters &) override;
        void store(const Photo::Id &, const ThumbnailParameters &, const QImage &) override;

    private:
        typedef QCache<std::tuple<Photo::Id, ThumbnailParameters>, QImage> CacheContainer;
        ol::ThreadSafeResource<CacheContainer> m_cache;
};

#endif
