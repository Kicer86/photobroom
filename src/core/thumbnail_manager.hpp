/*
 * Class responsible for managing thumbnails
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

#include <QImage>

#include "athumbnail_manager.hpp"
#include "core_export.h"


struct IThumbnailCache
{
    virtual ~IThumbnailCache() = default;

    virtual std::optional<QImage> find(const QString &, int) = 0;
    virtual void store(const QString &, int, const QImage &) = 0;
};


class CORE_EXPORT ThumbnailManager: public AThumbnailManager
{
    public:
        ThumbnailManager(AThumbnailGenerator *);

        void setCache(IThumbnailCache *);

    private:
        IThumbnailCache* m_cache;

        QImage find(const QString &, int);
        void cache(const QString &, int, const QImage &);
};

#endif // THUMBNAILMANAGER_HPP
