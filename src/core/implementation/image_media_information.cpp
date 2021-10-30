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


ImageMediaInformation::ImageMediaInformation(IExifReaderFactory& exif): m_exif(exif)
{

}


std::optional<QSize> ImageMediaInformation::size(const QString& path) const
{
    IExifReader& exif_reader = m_exif.get();

    std::optional<QSize> result;

    const bool has = exif_reader.hasExif(path);

    if (has)
    {
        // Here we could have used exif's
        // Exif.Photo.PixelYDimension or
        // Exif.Image.ImageWidth
        // kind of tags to get photo dimensions.
        // But it may happend that a photo was rotated in an editor
        // and exif was not touched. It will cause wrong results here.
        // Therefore QImageReader is used here.

        const QImageReader reader(path);
        QSize size = reader.size();

        const std::optional<std::any> orientation_raw = exif_reader.get(path, IExifReader::TagType::Orientation);

        int orientation = 0;
        if (orientation_raw.has_value())
            orientation = std::any_cast<int>(*orientation_raw);

        // orientations 5, 6, 7 and 8 require 90⁰ degree rotations which swap dimensions
        if (orientation > 4)
            size.transpose();

        result = size;
    }

    return result;
}
