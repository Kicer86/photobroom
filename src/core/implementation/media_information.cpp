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

#include "media_information.hpp"

#include <cassert>

#include <QFileInfo>
#include <QImageReader>

#include "icore_factory_accessor.hpp"
#include "ilogger_factory.hpp"
#include "ilogger.hpp"
#include "media_types.hpp"
#include "implementation/image_media_information.hpp"
#include "implementation/video_media_information.hpp"


namespace
{
    QSize imageSize(const QString& path)
    {
        const QImageReader reader(path);
        const QSize size = reader.size();

        return size;
    }
}


struct MediaInformation::Impl
{
    std::unique_ptr<ILogger> m_logger;
    ImageMediaInformation m_image_info;
    VideoMediaInformation m_video_info;

    explicit Impl(ICoreFactoryAccessor* coreFactory):
        m_logger(coreFactory->getLoggerFactory().get("Media Information")),
        m_image_info(coreFactory->getExifReaderFactory(), *m_logger),
        m_video_info(coreFactory->getConfiguration())
    {

    }
};


MediaInformation::MediaInformation(ICoreFactoryAccessor* coreFactory): m_impl(std::make_unique<Impl>(coreFactory))
{

}


MediaInformation::~MediaInformation()
{

}


std::optional<QSize> MediaInformation::size(const QString& path) const
{
    const QFileInfo fileInfo(path);
    const QString full_path = fileInfo.absoluteFilePath();

    std::optional<QSize> result = m_impl->m_image_info.size(full_path);      // try to use exif (so orientation will be considered)

    if (result.has_value() == false && MediaTypes::isImageFile(full_path))  // no exif, but image file - read its dimensions from image properties
    {
        const QSize imgSize = imageSize(full_path);
        if (imgSize.isValid())
            result = imgSize;
    }

    if (result.has_value() == false && MediaTypes::isVideoFile(full_path))  // still no data, and video file
        result = m_impl->m_video_info.size(full_path);

    if (result.has_value() == false)
    {
        const QString error = QString("Could not load image data from '%1'. File format unknown or file corrupted").arg(path);

        m_impl->m_logger->error(error);
    }

    return result;
}


FileInformation MediaInformation::getInformation(const QString& path) const
{
    FileInformation info;

    if (MediaTypes::isImageFile(path))
        info = m_impl->m_image_info.getInformation(path);
    else if (MediaTypes::isVideoFile(path))
        info = m_impl->m_video_info.getInformation(path);
    else
        m_impl->m_logger->error(QString("Unknown type of file: %1").arg(path));

    return info;
}
