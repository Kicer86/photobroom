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

#include <QFileInfo>

#include "icore_factory.hpp"
#include "ilogger_factory.hpp"
#include "ilogger.hpp"
#include "media_types.hpp"
#include "implementation/image_information.hpp"
#include "implementation/video_information.hpp"


struct MediaInformation::Impl
{
    ImageInformation m_image_info;
    VideoInformation m_video_info;
    std::unique_ptr<ILogger> m_logger;

    Impl(): m_image_info(), m_video_info(), m_logger(nullptr)
    {

    }
};


MediaInformation::MediaInformation(): m_impl(std::make_unique<Impl>())
{

}


MediaInformation::~MediaInformation()
{
}


void MediaInformation::set( ICoreFactoryAccessor* coreFactory)
{
    m_impl->m_image_info.set(coreFactory->getExifReaderFactory());
    m_impl->m_video_info.set(coreFactory->getConfiguration());
    m_impl->m_logger = coreFactory->getLoggerFactory()->get("Media Information");
}


QSize MediaInformation::size(const QString& path) const
{
    const QFileInfo fileInfo(path);
    const QString full_path = fileInfo.absoluteFilePath();

    QSize result;

    if (MediaTypes::isImageFile(full_path))
        result = m_impl->m_image_info.size(full_path);
    else if (MediaTypes::isVideoFile(full_path))
        result = m_impl->m_video_info.size(full_path);
    else
        assert(!"unknown file type");

    if (result.isValid() == false)
    {
        std::string error = "Could not load image data from '";
        error += path.toStdString();
        error += "'. File format unknown or file corrupted";

        m_impl->m_logger->error(error);
    }



    return result;
}
