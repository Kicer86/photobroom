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

#ifndef THUMBNAILACQUISITOR_HPP
#define THUMBNAILACQUISITOR_HPP

#include <mutex>

#include <QImage>

#include "ithumbnail_generator.hpp"


class ThumbnailAcquisitor
{
    public:
        typedef std::function<void(const ThumbnailInfo &, const QImage &)> Observer;

        ThumbnailAcquisitor(IThumbnailGenerator *, IThumbnailCache *);
        ThumbnailAcquisitor(const ThumbnailAcquisitor &) = delete;
        ~ThumbnailAcquisitor();
        ThumbnailAcquisitor& operator=(const ThumbnailAcquisitor &) = delete;

        void setInProgressThumbnail(const QImage &);
        void setObserver(const Observer &);

        QImage getThumbnail(const ThumbnailInfo &) const;

    private:
        std::vector<Observer> m_observers;
        IThumbnailGenerator::Callback m_callback;
        QImage m_inProgress;
        mutable std::mutex m_cacheAccessMutex;
        IThumbnailGenerator* m_generator;
        IThumbnailCache* m_cache;

        void gotThumbnail(const ThumbnailInfo &, const QImage &);
};

#endif // THUMBNAILACQUISITOR_HPP
