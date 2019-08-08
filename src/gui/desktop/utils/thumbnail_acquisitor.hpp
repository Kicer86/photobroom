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

#include <core/exif_reader_factory.hpp>
#include "thumbnail_generator.hpp"
#include "ithumbnail_acquisitor.hpp"

struct IConfiguration;


class [[__deprecated__]] ThumbnailAcquisitor: public IThumbnailAcquisitor
{
    public:
        typedef std::function<void(const ThumbnailInfo &, const QImage &)> Observer;

        ThumbnailAcquisitor();
        ThumbnailAcquisitor(const ThumbnailAcquisitor &) = delete;
        ~ThumbnailAcquisitor();
        ThumbnailAcquisitor& operator=(const ThumbnailAcquisitor &) = delete;

        void set(ITaskExecutor *);
        void set(ILogger *);
        void set(IConfiguration *);

        void setInProgressThumbnail(const QImage &);
        void setObserver(const Observer &);
        void dismissPendingTasks();
        void flush();

        QImage getThumbnail(const ThumbnailInfo &) const override;

    private:
        std::vector<Observer> m_observers;
        QImage m_inProgress;
        mutable std::mutex m_cacheAccessMutex;
        ThumbnailGenerator m_generator;
        mutable ThumbnailCache m_cache;
        mutable std::set<ThumbnailInfo> m_awaitingTasks;
        mutable ExifReaderFactory m_exifFactory;

        void gotThumbnail(const ThumbnailInfo &, const QImage &) const;
};

#endif // THUMBNAILACQUISITOR_HPP
