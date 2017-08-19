/*
 * Toolkit for reading media file size
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

#include "media_information.hpp"

#include <cassert>

#include "media_types.hpp"
#include "implementation/image_information.hpp"
#include "implementation/video_information.hpp"


struct MediaInformation::Impl
{
    ImageInformation m_image_info;
    VideoInformation m_video_info;

    Impl(): m_image_info(), m_video_info()
    {

    }
};


MediaInformation::MediaInformation(): m_impl(std::make_unique<Impl>())
{

}


MediaInformation::~MediaInformation()
{
}


void MediaInformation::set(IExifReaderFactory* exif)
{
    m_impl->m_image_info.set(exif);
}


QSize MediaInformation::size(const QString& path) const
{
    QSize result;

    if (MediaTypes::isImageFile(path))
        result = m_impl->m_image_info.size(path);
    else if (MediaTypes::isVideoFile(path))
        result = m_impl->m_video_info.size(path);
    else
        assert(!"unknown file type");

    return result;
}
