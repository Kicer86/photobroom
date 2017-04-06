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

#include "photo_information.hpp"

#include <QImageReader>

#include <boost/any.hpp>

#include "iexif_reader.hpp"


PhotoInformation::PhotoInformation(): m_exif(nullptr)
{
}


PhotoInformation::~PhotoInformation()
{
}


QSize PhotoInformation::size(const QString& path) const
{
    IExifReader* exif_reader = m_exif->get();

    const QImageReader reader(path);
    QSize size = reader.size();

    const boost::any orientation_raw = exif_reader->get(path, IExifReader::TagType::Orientation);
    const int orientation = boost::any_cast<int>(orientation_raw);

    if (orientation > 4) // orientations 5, 6, 7 and 8 require 90⁰ degree rotations which swap dimensions
        size.transpose();

    return size;
}


void PhotoInformation::set(IExifReaderFactory* exif)
{
    m_exif = exif;
}
