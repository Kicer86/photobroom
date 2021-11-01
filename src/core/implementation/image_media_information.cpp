/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
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

#include "image_media_information.hpp"

#include <any>

#include <QImageReader>

#include "iexif_reader.hpp"


ImageMediaInformation::ImageMediaInformation(IExifReaderFactory& exif, ILogger& logger)
    : m_exif(exif)
    , m_logger(logger)
{

}


FileInformation ImageMediaInformation::getInformation(const QString& path) const
{
    IExifReader& exif = m_exif.get();

    FileInformation info;
    info.common.dimension = size(path, exif);
    info.common.creationTime = creationTime(path, exif);

    return info;
}


std::optional<QSize> ImageMediaInformation::size(const QString& path, IExifReader& exif) const
{
    // Here we could have used exif's
    // Exif.Photo.PixelYDimension or
    // Exif.Image.ImageWidth
    // kind of tags to get photo dimensions (if exif available).
    // But it may happend that a photo was rotated in an editor
    // and exif's dimensions were not touched. It will cause wrong results.
    // Therefore QImageReader is used here as basic source of image resolution.

    std::optional<QSize> result;

    const QImageReader reader(path);
    const QSize size = reader.size();

    if (size.isValid())
        result = size;

    const bool has = exif.hasExif(path);

    if (has)
    {
        const std::optional<std::any> xdim = exif.get(path, IExifReader::TagType::PixelXDimension);
        const std::optional<std::any> ydim = exif.get(path, IExifReader::TagType::PixelYDimension);

        if (xdim.has_value() && ydim.has_value())
        {
            const long x = std::any_cast<long>(*xdim);
            const long y = std::any_cast<long>(*ydim);

            // use exif if QImageReader could not handle image
            if (!result)
                result = QSize(x, y);
            else if (x != result->width() || y != result->height())   // perform validation for debug purposes
                m_logger.warning(
                    QString("For photo %5 exif data are inconsistent with image data: Image reports %1x%2 resolution while exif reports: %3x%4.")
                    .arg(result->width())
                    .arg(result->height())
                    .arg(x)
                    .arg(y)
                    .arg(path)
                );
        }

        // apply orientation if available
        const std::optional<std::any> orientation_raw = exif.get(path, IExifReader::TagType::Orientation);

        int orientation = 0;
        if (orientation_raw.has_value())
            orientation = std::any_cast<int>(*orientation_raw);

        // orientations 5, 6, 7 and 8 require 90⁰ degree rotations which swap dimensions
        const bool needsRotation = orientation > 4;

        if (needsRotation && result)
            result->transpose();
    }

    return result;
}


std::optional<QDateTime> ImageMediaInformation::creationTime(const QString& path, IExifReader& exif) const
{
    std::optional<QDateTime> result;

    const auto datetime_raw = exif.get(path, IExifReader::TagType::DateTimeOriginal);

    if (datetime_raw.has_value())
    {
        const auto datetime = std::any_cast<std::string>(*datetime_raw);
        const QString v(datetime.c_str());
        const QStringList time_splitted = v.split(" ");

        if (time_splitted.size() == 2)
        {
            QString date_raw = time_splitted[0];
            const QString time_raw = time_splitted[1];

            const QDate date = QDate::fromString(date_raw, "yyyy:MM:dd");
            const QTime time = QTime::fromString(time_raw, "hh:mm:ss");
            const QDateTime datetime(date, time);

            if (datetime.isValid())
                result = datetime;
            else
                m_logger.warning(QString("File %1 contains broken exif data").arg(path));
        }
    }

    return result;
}
