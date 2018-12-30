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

#include "icore_factory_accessor.hpp"
#include "ilogger_factory.hpp"
#include "ilogger.hpp"
#include "media_types.hpp"
#include "implementation/exiv2_media_information.hpp"
#include "implementation/ffmpeg_media_information.hpp"


struct MediaInformation::Impl
{
    Eviv2MediaInformation m_exif_info;
    FFmpegMediaInformation m_ffmpeg_info;
    std::unique_ptr<ILogger> m_logger;

    Impl(): m_exif_info(), m_ffmpeg_info(), m_logger(nullptr)
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
    m_impl->m_exif_info.set(coreFactory->getExifReaderFactory());
    m_impl->m_ffmpeg_info.set(coreFactory->getConfiguration());
    m_impl->m_logger = coreFactory->getLoggerFactory()->get("Media Information");
}


bool MediaInformation::canHandle(const QString& path) const
{
    return m_impl->m_exif_info.canHandle(path) ||
           m_impl->m_ffmpeg_info.canHandle(path);
}


QSize MediaInformation::size(const QString& path) const
{
    const QFileInfo fileInfo(path);
    const QString full_path = fileInfo.absoluteFilePath();

    QSize result;

    if (m_impl->m_exif_info.canHandle(full_path))
        result = m_impl->m_exif_info.size(full_path);
    else if (MediaTypes::isVideoFile(full_path))
        result = m_impl->m_ffmpeg_info.size(full_path);

    if (result.isValid() == false)
    {
        std::string error = "Could not load image data from '";
        error += path.toStdString();
        error += "'. File format unknown or file corrupted";

        m_impl->m_logger->error(error);
    }



    return result;
}
