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

#include <core/task_executor.hpp>
#include <core/iphotos_manager.hpp>

namespace
{

    struct ThumbnailGeneratorTask: TaskExecutor::ITask
    {
        ThumbnailGeneratorTask(const ThumbnailInfo& info,
                               const ThumbnailGenerator::Callback& callback,
                               IPhotosManager* photosManager):
            m_info(info),
            m_callback(callback),
            m_photosManager(photosManager)
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
            QByteArray raw = m_photosManager->getPhoto(m_info.path);

            QImage result;
            result.loadFromData(raw);

            if (result.height() != m_info.height)
                result = result.scaledToHeight(m_info.height, Qt::SmoothTransformation);

            m_callback(m_info, result);
        }

        ThumbnailInfo m_info;
        ThumbnailGenerator::Callback m_callback;
        IPhotosManager* m_photosManager;
    };

}

uint qHash(const ThumbnailInfo& key, uint seed = 0)
{
    return qHash(key.path) ^ qHash(key.height) ^ seed;
}


ThumbnailGenerator::ThumbnailGenerator(): m_tasks(), m_executor(nullptr), m_photosManager(nullptr)
{

}


ThumbnailGenerator::~ThumbnailGenerator()
{

}


void ThumbnailGenerator::dismissPedingTasks()
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


void ThumbnailGenerator::generateThumbnail(const ThumbnailInfo& info, const Callback& callback) const
{
    auto task = std::make_unique<ThumbnailGeneratorTask>(info, callback, m_photosManager);
    m_tasks->push(std::move(task));
}



ThumbnailCache::ThumbnailCache():
    m_cacheMutex(),
    m_cache(256)
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

