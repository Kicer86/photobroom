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

#include "thumbnail_manager.hpp"

#include "ithumbnails_cache.hpp"

using namespace std::placeholders;


ThumbnailManager::ThumbnailManager(ITaskExecutor* executor, IThumbnailsGenerator& gen, IThumbnailsCache& cache):
    m_tasks(executor, TasksQueue::Mode::Lifo),
    m_cache(cache),
    m_generator(gen)
{
}


void ThumbnailManager::fetch(const QString& path, const QSize& desired_size, const std::function<void(const QImage &)>& callback)
{
    internal_fetch(path, IThumbnailsCache::ThumbnailParameters(desired_size), callback);
}


std::optional<QImage> ThumbnailManager::fetch(const QString& path, const QSize& desired_size)
{
    std::optional img = m_cache.find(path, IThumbnailsCache::ThumbnailParameters(desired_size));

    return img;
}


QImage ThumbnailManager::find(const QString& path, const IThumbnailsCache::ThumbnailParameters& params)
{
    QImage result;

    const auto cached = m_cache.find(path, params);

    if (cached.has_value())
        result = *cached;

    return result;
}


void ThumbnailManager::cache(const QString& path, const IThumbnailsCache::ThumbnailParameters& params, const QImage& img)
{
    m_cache.store(path, params, img);
}


void ThumbnailManager::generate(const QString& path, const IThumbnailsCache::ThumbnailParameters& params, const std::function<void(const QImage &)>& callback)
{
    runOn(m_tasks, [=, this]
    {
        generate_task(path, params, callback);
    }, "ThumbnailManager::generate");
}

