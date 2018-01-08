/*
 * Thumbnail Acquisitor
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


#include "thumbnail_acquisitor.hpp"

#include <core/iexif_reader.hpp>


ThumbnailAcquisitor::ThumbnailAcquisitor():
    m_observers(),
    m_inProgress(),
    m_cacheAccessMutex(),
    m_generator(),
    m_cache(),
    m_awaitingTasks(),
    m_exifFactory()
{
    m_generator.set(&m_exifFactory);
}


ThumbnailAcquisitor::~ThumbnailAcquisitor()
{

}


void ThumbnailAcquisitor::set(ITaskExecutor* executor)
{
    m_generator.set(executor);
}


void ThumbnailAcquisitor::set(ILogger* logger)
{
    m_generator.set(logger);
}


void ThumbnailAcquisitor::set(IConfiguration* config)
{
    m_generator.set(config);
}


void ThumbnailAcquisitor::setInProgressThumbnail(const QImage& image)
{
    m_inProgress = image;
}


void ThumbnailAcquisitor::setObserver(const Observer& observer)
{
    m_observers.push_back(observer);
}


void ThumbnailAcquisitor::dismissPendingTasks()
{
    std::lock_guard<std::mutex> lock(m_cacheAccessMutex);

    m_awaitingTasks.clear();
    m_generator.dismissPendingTasks();
}


void ThumbnailAcquisitor::flush()
{
    std::unique_lock<std::mutex> lock(m_cacheAccessMutex);

    m_cache.clear();
}


QImage ThumbnailAcquisitor::getThumbnail(const ThumbnailInfo& info) const
{
    QImage result;

    std::unique_lock<std::mutex> lock(m_cacheAccessMutex);

    auto awaiting = m_awaitingTasks.find(info);

    if (awaiting != m_awaitingTasks.end())
        result = m_inProgress;
    else
    {
        auto image = m_cache.get(info);

        if (image)
            result = *image;
        else
        {
            m_awaitingTasks.insert(info);
            result = m_inProgress;

            lock.unlock();   // lock is not needed and can cause deadlock if generator calls ThumbnailAcquisitor::gotThumbnail directly
            auto callback = std::bind(&ThumbnailAcquisitor::gotThumbnail, this, std::placeholders::_1, std::placeholders::_2);
            m_generator.generateThumbnail(info, callback);
        }
    }

    return result;
}


void ThumbnailAcquisitor::gotThumbnail(const ThumbnailInfo& info, const QImage& image) const
{
    std::lock_guard<std::mutex> lock(m_cacheAccessMutex);

    // It is possible to get thumbnail which was not awaited.
    // m_awaitingTasks could be cleared when generation task was being executed.
    m_awaitingTasks.erase(info);

    m_cache.add(info, image);

    for(const Observer& obs: m_observers)
        obs(info, image);
}
