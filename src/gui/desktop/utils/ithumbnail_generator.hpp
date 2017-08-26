/*
 * Interface for thumbnails generator and cache.
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

#ifndef ITHUMBNAILSGENERATOR_HPP
#define ITHUMBNAILSGENERATOR_HPP

#include <functional>
#include <optional>

#include <QImage>
#include <QString>



struct ThumbnailInfo
{
    QString path;
    int height;

    bool operator==(const ThumbnailInfo& other) const
    {
        return path == other.path &&
               height == other.height;
    }

    bool operator<(const ThumbnailInfo& other) const
    {
        if (path < other.path)
            return true;
        else if (path == other.path)
            return height < other.height;
        else
            return false;
    }
};


struct IThumbnailGenerator
{
    // TODO: ThumbnailInfo here is not necessary. One can use proper std::binding if needs it
    typedef std::function<void(const ThumbnailInfo &, const QImage &)> Callback;

    virtual ~IThumbnailGenerator() {}

    virtual void generateThumbnail(const ThumbnailInfo &, const Callback &) const = 0;
};


struct IThumbnailCache
{
    virtual ~IThumbnailCache() {}

    virtual std::optional<QImage> get(const ThumbnailInfo &) const = 0;
    virtual void add(const ThumbnailInfo &, const QImage &) = 0;
};

#endif // ITHUMBNAILSGENERATOR_HPP
