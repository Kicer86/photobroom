/*
 * Class responsible for managing thumbnails
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

#ifndef THUMBNAILMANAGER_HPP
#define THUMBNAILMANAGER_HPP

#include <memory>
#include <optional>

#include <QImage>

#include "ithumbnails_cache.hpp"
#include "ithumbnails_manager.hpp"
#include "core_export.h"


struct IThumbnailsCache;

class CORE_EXPORT ThumbnailManager: public IThumbnailsManager
{
    public:
        explicit ThumbnailManager(IThumbnailsGenerator *, IThumbnailsCache * = nullptr);

        void setCache(IThumbnailsCache *);

        // call 'callback' when thumbnail is ready (it may happend immediately when found in cache)
        void fetch(const QString& path, int desired_height, const std::function<void(int, const QImage &)> &);

        // return required thumbnail (if in cache).
        // Otherwise returns empty result and generates thumbnail in background for later use.
        std::optional<QImage> fetch(const QString& path, int height);

    private:
        IThumbnailsCache* m_cache;
        IThumbnailsGenerator* m_generator;

        QImage find(const QString &, int);
        void cache(const QString &, int, const QImage &);

        template<typename C>
        void generate(const QString& path, int desired_height, C&& callback)
        {
            // TODO: move to thread
            const QImage img = m_generator->generate(path, desired_height);

            const int height = img.height();
            assert(height == desired_height || img.isNull());

            cache(path, height, img);
            callback(height, img);
        }
};

#endif // THUMBNAILMANAGER_HPP

