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

#ifndef THUMBNAILMANAGER_HPP
#define THUMBNAILMANAGER_HPP

#include <memory>
#include <optional>

#include <QImage>

#include "ithumbnails_cache.hpp"
#include "ithumbnails_manager.hpp"
#include "task_executor_utils.hpp"

#include "core_export.h"


struct ITaskExecutor;
struct IThumbnailsCache;


class CORE_EXPORT ThumbnailManager: public IThumbnailsManager
{
    public:
        explicit ThumbnailManager(ITaskExecutor *, IThumbnailsGenerator &, IThumbnailsCache &);

        void fetch(const QString& path, int desired_height, const std::function<void(const QImage &)> &) override;
        void fetch(const QString& path, int desired_height, const safe_callback<const QImage &> &) override;
        std::optional<QImage> fetch(const QString& path, int height) override;

    private:
        TasksQueue m_tasks;
        IThumbnailsCache& m_cache;
        IThumbnailsGenerator& m_generator;

        QImage find(const QString &, int);
        void cache(const QString &, int, const QImage &);

        void generate(const QString &, int, const safe_callback<const QImage &> &);
        void generate(const QString &, int, const std::function<void(const QImage &)> &);

        template<typename T>
        void internal_fetch(const QString& path, int desired_height, const T& callback)
        {
            const QImage cached = find(path, desired_height);

            if (cached.isNull())
                generate(path, desired_height, callback);
            else
                callback(cached);
        }

        template<typename T>
        void generate_task(const QString& path, int desired_height, const T& callback)
        {
            const QImage img = m_generator.generate(path, desired_height);

            const int height = img.height();
            assert(height == desired_height || img.isNull());

            cache(path, desired_height, img);
            callback(img);
        }
};

#endif // THUMBNAILMANAGER_HPP

