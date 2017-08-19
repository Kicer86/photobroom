/*
 * Toolkit for reading video information
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

#include "video_information.hpp"

#include <QMediaContent>
#include <QString>


VideoInformation::VideoInformation()
{
}


QSize VideoInformation::size(const QString& path) const
{
    const QUrl url(path);
    const QMediaContent mediaContent(url);
    const QMediaResource resource = mediaContent.canonicalResource();
    const QSize resolution = resource.resolution();

    return resolution;
}
