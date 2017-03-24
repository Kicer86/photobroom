/*
 * Low level thumbnails generator and catcher.
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

#include "thumbnail_generator.hpp"

#include <core/ilogger.hpp>
#include <core/iphotos_manager.hpp>
#include <core/stopwatch.hpp>
#include <core/task_executor.hpp>

namespace
{

    struct ThumbnailGeneratorTask: TaskExecutor::ITask
    {
        ThumbnailGeneratorTask(const ThumbnailInfo& info,
                               const ThumbnailGenerator::Callback& callback,
                               IPhotosManager* photosManager,
                               ILogger* logger):
            m_info(info),
            m_callback(callback),
            m_photosManager(photosManager),
            m_logger(logger)
        {

        }

        virtual ~ThumbnailGeneratorTask() {}

        ThumbnailGeneratorTask(const ThumbnailGeneratorTask &) = delete;
        ThumbnailGeneratorTask& operator=(const ThumbnailGeneratorTask &) = delete;

        virtual std::string name() const override
        {
            return "Photo thumbnail generation";
        }

        virtual void perform() override
        {
            Stopwatch stopwatch;

            stopwatch.start();
            QByteArray raw = m_photosManager->getPhoto(m_info.path);

            QImage result;
            result.loadFromData(raw);
            const int photo_read = stopwatch.read(true);

            if (result.height() != m_info.height)
                result = result.scaledToHeight(m_info.height, Qt::SmoothTransformation);

            const int photo_scaling = stopwatch.stop();

            const std::string read_time_message = std::string("photo read time: ") + std::to_string(photo_read) + "ms";
            m_logger->debug(read_time_message);

            const std::string scaling_time_message = std::string("photo scaling time: ") + std::to_string(photo_scaling) + "ms";
            m_logger->debug(scaling_time_message);

            m_callback(m_info, result);
        }

        ThumbnailInfo m_info;
        ThumbnailGenerator::Callback m_callback;
        IPhotosManager* m_photosManager;
        ILogger* m_logger;
    };

}

uint qHash(const ThumbnailInfo& key, uint seed = 0)
{
    return qHash(key.path) ^ qHash(key.height) ^ seed;
}


ThumbnailGenerator::ThumbnailGenerator(): m_tasks(), m_executor(nullptr), m_photosManager(nullptr), m_logger(nullptr)
{

}


ThumbnailGenerator::~ThumbnailGenerator()
{

}


void ThumbnailGenerator::dismissPendingTasks()
{
    m_tasks->clear();
}


void ThumbnailGenerator::set(ITaskExecutor* executor)
{
    m_executor = executor;

    m_tasks = std::move( m_executor->getCustomTaskQueue() );
}


void ThumbnailGenerator::set(IPhotosManager* photosManager)
{
    m_photosManager = photosManager;
}


void ThumbnailGenerator::set(ILogger* logger)
{
    m_logger = logger;
}


void ThumbnailGenerator::generateThumbnail(const ThumbnailInfo& info, const Callback& callback) const
{
    auto task = std::make_unique<ThumbnailGeneratorTask>(info, callback, m_photosManager, m_logger);
    m_tasks->push(std::move(task));
}



ThumbnailCache::ThumbnailCache():
    m_cacheMutex(),
    m_cache(2048)
{

}


ThumbnailCache::~ThumbnailCache()
{

}


void ThumbnailCache::add(const ThumbnailInfo& info, const QImage& img)
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);

    QImage* new_img = new QImage(img);

    m_cache.insert(info, new_img);
}


boost::optional<QImage> ThumbnailCache::get(const ThumbnailInfo& info) const
{
    boost::optional<QImage> result;

    std::lock_guard<std::mutex> lock(m_cacheMutex);

    if (m_cache.contains(info))
        result = *m_cache[info];

    return result;
}

