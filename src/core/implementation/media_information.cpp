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


struct MediaInformation::Impl
{
    std::unique_ptr<ILogger> m_logger;
    ImageMediaInformation m_image_info;
    VideoMediaInformation m_video_info;

    explicit Impl(ICoreFactoryAccessor* coreFactory):
        m_logger(coreFactory->getLoggerFactory().get("Media Information")),
        m_image_info(coreFactory->getExifReaderFactory(), *m_logger),
        m_video_info(coreFactory->getExifReaderFactory(), *m_logger)
    {

    }
};


MediaInformation::MediaInformation(ICoreFactoryAccessor* coreFactory): m_impl(std::make_unique<Impl>(coreFactory))
{

}


MediaInformation::~MediaInformation()
{

}


FileInformation MediaInformation::getInformation(const QString& path) const
{
    FileInformation info;
    const QFileInfo fileInfo(path);
    const QString full_path = fileInfo.absoluteFilePath();

    if (MediaTypes::isImageFile(full_path))
        info = m_impl->m_image_info.getInformation(full_path);
    else if (MediaTypes::isVideoFile(full_path))
        info = m_impl->m_video_info.getInformation(full_path);
    else
        m_impl->m_logger->error(QString("Unknown type of file: %1").arg(full_path));

    return info;
}
