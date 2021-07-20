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
#include <core/task_executor_utils.hpp>
#include <core/ithumbnails_generator.hpp>
#include <database/idatabase.hpp>

#include "ithumbnails_cache.hpp"
#include "ithumbnails_manager.hpp"

struct ITaskExecutor;
struct IThumbnailsCache;


class ThumbnailManager: public IThumbnailsManager
{
    public:
        explicit ThumbnailManager(ITaskExecutor *, IThumbnailsGenerator &, IThumbnailsCache &, Database::IDatabase * = nullptr);

        void fetch(const Photo::Id& id, const QSize& desired_size, const std::function<void(const QImage &)> &) override;
        std::optional<QImage> fetch(const Photo::Id& id, const QSize& desired_size) override;

        void setDatabaseCache(Database::IDatabase *) override;

    private:
        TasksQueue m_tasks;
        IThumbnailsCache& m_cache;
        IThumbnailsGenerator& m_generator;
        Database::IDatabase* m_db;

        QImage find(const Photo::Id &, const IThumbnailsCache::ThumbnailParameters &);
        void cache(const Photo::Id &, const IThumbnailsCache::ThumbnailParameters &, const QImage &);
};

#endif // THUMBNAILMANAGER_HPP

