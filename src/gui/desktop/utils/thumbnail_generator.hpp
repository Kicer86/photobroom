/*
 * Thumbnail generator.
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef THUMBNAILGENERATOR_HPP
#define THUMBNAILGENERATOR_HPP

#include <mutex>

#include <QCache>

#include <core/ts_multi_queue.hpp>
#include <core/task_executor.hpp>

#include "ithumbnail_generator.hpp"


struct IExifReaderFactory;
struct IConfiguration;


class ThumbnailGenerator: public IThumbnailGenerator
{
    public:
        ThumbnailGenerator();
        ThumbnailGenerator(const ThumbnailGenerator &) = delete;
        ~ThumbnailGenerator();

        ThumbnailGenerator& operator=(const ThumbnailGenerator &) = delete;

        void dismissPendingTasks();
        void set(ITaskExecutor *);
        void set(ILogger *);
        void set(IExifReaderFactory *);
        void set(IConfiguration *);

        // IThumbnailGenerator:
        void generateThumbnail(const ThumbnailInfo &, const Callback &) const override;

    private:
        QImage m_videoImage;
        ITaskExecutor::TaskQueue m_tasks;
        ITaskExecutor* m_executor;
        ILogger* m_logger;
        IExifReaderFactory* m_exifReaderFactory;
        IConfiguration* m_configuration;

        struct FromImageTask;
        friend struct FromImageTask;

        struct FromVideoTask;
        friend struct FromVideoTask;
};


class ThumbnailCache: public IThumbnailCache
{
    public:
        ThumbnailCache();
        ThumbnailCache(const ThumbnailCache &) = delete;
        ~ThumbnailCache();

        ThumbnailCache& operator=(const ThumbnailCache &) = delete;

        void add(const ThumbnailInfo &, const QImage &) override;
        std::optional<QImage> get(const ThumbnailInfo &) const override;
        void clear() override;

    private:
        mutable std::mutex m_cacheMutex;
        QCache<ThumbnailInfo, QImage> m_cache;
};

#endif // THUMBNAILGENERATOR_HPP
