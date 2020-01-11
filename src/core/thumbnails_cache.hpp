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

#include "ithumbnails_cache.hpp"

#include <QCache>

#include <OpenLibrary/putils/ts_resource.hpp>

#include "core_export.h"

class CORE_EXPORT ThumbnailsCache: public IThumbnailsCache
{
    public:
        ThumbnailsCache();

        std::optional<QImage> find(const QString &, int) override;
        void store(const QString &, int , const QImage &) override;

    private:
        typedef QCache<std::tuple<QString, int>, QImage> CacheContainer;
        ol::ThreadSafeResource<CacheContainer> m_cache;
};

#endif
