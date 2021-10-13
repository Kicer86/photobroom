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


#include <any>
#include <QFileInfo>
#include <QImageReader>

#include <core/iexif_reader.hpp>
#include "oriented_image.hpp"


OrientedImage::OrientedImage():
    m_oriented()
{
}


OrientedImage::OrientedImage(IExifReader& exif, const QString& src):
    m_oriented()
{
    QImage img(src);
    QImage rotated;

    if (img.isNull())
    {
        QImageReader check(src);
        qDebug() << check.errorString();
    }
    else
    {
        const std::optional<std::any> orientation_raw = exif.get(src, IExifReader::TagType::Orientation);
        const int orientation = orientation_raw.has_value()?
                                    std::any_cast<int>(*orientation_raw):
                                    0;

        switch(orientation)
        {
            case 0:
            case 1:
                rotated = img;
                break;    // nothing to do - no data, or normal orientation

            case 2:
                rotated = img.mirrored(true, false);
                break;

            case 3:
            {
                QTransform transform;
                transform.rotate(180);

                rotated = img.transformed(transform, Qt::SmoothTransformation);
                break;
            }

            case 4:
                rotated = img.mirrored(false, true);
                break;

            case 5:
            {
                QTransform transform;
                transform.rotate(270);

                rotated = img.mirrored(true, false).transformed(transform);
                break;
            }

            case 6:
            {
                QTransform transform;
                transform.rotate(90);

                rotated = img.transformed(transform, Qt::SmoothTransformation);
                break;
            }

            case 7:
            {
                QTransform transform;
                transform.rotate(90);

                rotated = img.mirrored(true, false).transformed(transform);
                break;
            }

            case 8:
            {
                QTransform transform;
                transform.rotate(270);

                rotated = img.transformed(transform);
                break;
            }
        }
    }

    m_oriented = rotated;
}


QImage OrientedImage::get() const
{
    return m_oriented;
}


const QImage* OrientedImage::operator->() const
{
    return &m_oriented;
}
