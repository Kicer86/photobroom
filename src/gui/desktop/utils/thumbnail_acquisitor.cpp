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

ThumbnailAcquisitor::ThumbnailAcquisitor(IThumbnailGenerator* generator, IThumbnailCache* cache):
    m_observers(),
    m_callback(),
    m_inProgress(),
    m_cacheAccessMutex(),
    m_generator(generator),
    m_cache(cache)
{
    m_callback =  std::bind(&ThumbnailAcquisitor::gotThumbnail, this, std::placeholders::_1, std::placeholders::_2);
}


ThumbnailAcquisitor::~ThumbnailAcquisitor()
{

}


void ThumbnailAcquisitor::setInProgressThumbnail(const QImage& image)
{
    m_inProgress = image;
}


void ThumbnailAcquisitor::setObserver(const Observer& observer)
{
    m_observers.push_back(observer);
}


QImage ThumbnailAcquisitor::getThumbnail(const ThumbnailInfo& info) const
{
    QImage result;

    std::lock_guard<std::mutex> lock(m_cacheAccessMutex);

    auto image = m_cache->get(info);

    if (image)
        result = *image;
    else
    {
        // store temporary image in cache,
        // so new requsts for it will not call generation
        m_cache->add(info, m_inProgress);

        m_generator->generateThumbnail(info, m_callback);
    }

    return result;
}


void ThumbnailAcquisitor::gotThumbnail(const ThumbnailInfo& info, const QImage& image)
{
    std::lock_guard<std::mutex> lock(m_cacheAccessMutex);

    m_cache->add(info, image);

    for(const Observer& obs: m_observers)
        obs(info, image);
}
