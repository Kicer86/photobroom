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

#ifndef THUMBNAILGENERATOR_HPP
#define THUMBNAILGENERATOR_HPP

#include "core_export.h"
#include "exif_reader_factory.hpp"
#include "ithumbnails_generator.hpp"


struct IConfiguration;
struct IExifReaderFactory;
struct ILogger;


class CORE_EXPORT ThumbnailGenerator: public IThumbnailsGenerator
{
    public:
        ThumbnailGenerator(ILogger *, IConfiguration *);
        ThumbnailGenerator(const ThumbnailGenerator &) = delete;
        ~ThumbnailGenerator();

        ThumbnailGenerator& operator=(const ThumbnailGenerator &) = delete;

        // IThumbnailGenerator:
        QImage generate(const QString &, const ThumbnailParameters& params) override;
        QImage generateFrom(const QImage &, const ThumbnailParameters& params) override;

    private:
        ILogger* m_logger;
        mutable ExifReaderFactory m_exifReaderFactory;
        IConfiguration* m_configuration;

        QImage readFrameFromImage(const QString& path) const;
        QImage readFrameFromVideo(const QString& path, const QString& ffprobe, const QString& ffmpeg) const;
        QImage readFrame(const QString& path) const;
        QImage scaleImage(const QImage& path, const ThumbnailParameters& params) const;
};

#endif // THUMBNAILGENERATOR_HPP
