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

#include <QImageReader>
#include <QMimeDatabase>

#include "iexif_reader.hpp"


namespace
{
    // http://dev.exiv2.org/projects/exiv2/wiki/Supported_image_formats
    // TODO: http://dev.exiv2.org/projects/exiv2/wiki/Supported_video_formats
    const QStringList supportedImageTypes =
    {
        "image/jpeg",
        "image/x-exv",
        "image/x-canon-cr2",
        "image/x-canon-crw",
        "image/x-minolta-mrw",
        "image/tiff",
        "image/webp",
        "image/tiff (*)",
        "image/x-nikon-nef",
        "image/x-pentax-pef",
        "image/x-panasonic-rw2",
        "image/x-samsung-srw",
        "image/x-olympus-orf",
        "image/png",
        "image/pgf",
        "image/x-fuji-raf",
        "application/postscript",
        "application/rdf+xml",
        "image/gif",
        "image/x-photoshop",
        "image/targa",
        "image/x-ms-bmp",
        "image/jp2",
    };
}


Eviv2MediaInformation::Eviv2MediaInformation(): m_exif(nullptr)
{

}


Eviv2MediaInformation::~Eviv2MediaInformation()
{
}


bool Eviv2MediaInformation::canHandle(const QString& path) const
{
    const QMimeType file_mime_type = QMimeDatabase().mimeTypeForFile(path);
    bool matches = false;

    for(const QString& mimeType: supportedImageTypes)
    {
        matches = file_mime_type.inherits(mimeType);

        if (matches)
            break;
    }

    return matches;
}


QSize Eviv2MediaInformation::size(const QString& path) const
{
    IExifReader* exif_reader = m_exif->get();

    const QImageReader reader(path);
    QSize size = reader.size();

    const std::any orientation_raw = exif_reader->get(path, IExifReader::TagType::Orientation);
    const int orientation = std::any_cast<int>(orientation_raw);

    if (orientation > 4) // orientations 5, 6, 7 and 8 require 90⁰ degree rotations which swap dimensions
        size.transpose();

    return size;
}


void Eviv2MediaInformation::set(IExifReaderFactory* exif)
{
    m_exif = exif;
}
