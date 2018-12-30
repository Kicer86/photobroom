/*
 * Toolkit for reading information from photos
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

#include "exiv2_media_information.hpp"

#include <any>

#include "iexif_reader.hpp"


Eviv2MediaInformation::Eviv2MediaInformation(): m_exif(nullptr)
{

}


Eviv2MediaInformation::~Eviv2MediaInformation()
{
}


bool Eviv2MediaInformation::canHandle(const QString& path) const
{
    IExifReader* exif_reader = m_exif->get();
    const bool h = exif_reader->hasExif(path);
    return h;
}


QSize Eviv2MediaInformation::size(const QString& path) const
{
    IExifReader* exif_reader = m_exif->get();

    const std::any x_raw = exif_reader->get(path, IExifReader::TagType::PixelXDimension);
    const std::any y_raw = exif_reader->get(path, IExifReader::TagType::PixelYDimension);

    const long x = std::any_cast<long>(x_raw);
    const long y = std::any_cast<long>(y_raw);

    const QSize size(x, y);

    return size;
}


void Eviv2MediaInformation::set(IExifReaderFactory* exif)
{
    m_exif = exif;
}
