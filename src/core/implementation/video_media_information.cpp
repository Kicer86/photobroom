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


#include <cassert>

#include <opencv2/opencv.hpp>

#include "constants.hpp"
#include "iconfiguration.hpp"
#include "exiftool_video_details_reader.hpp"
#include "video_media_information.hpp"


VideoMediaInformation::VideoMediaInformation(IConfiguration& configuration, const ILogger& logger)
    : m_logger(logger.subLogger("VideoMediaInformation"))
{
    const QVariant exiftoolVar = configuration.getEntry(ExternalToolsConfigKeys::exiftoolPath);

    m_exiftoolPath = exiftoolVar.toString();
}


FileInformation VideoMediaInformation::getInformation(const QString& path) const
{
    const auto output = ExiftoolUtils::exiftoolOutput(m_exiftoolPath, path);
    const auto parsed = ExiftoolUtils::parseOutput(output);
    const ExiftoolVideoDetailsReader videoDetailsReader(parsed);

    FileInformation info;
    info.common.dimension = videoDetailsReader.resolutionOf();
    info.common.creationTime = videoDetailsReader.creationTime();

    m_logger->trace(QString("Opening video file %1 for reading video details").arg(path));
    cv::VideoCapture video(path.toStdString(), cv::CAP_FFMPEG);
    if (video.isOpened())
    {
        m_logger->trace("File opened successfully");

        const qint64 lenght = static_cast<int64>(video.get(cv::CAP_PROP_FRAME_COUNT) / video.get(cv::CAP_PROP_FPS) * 1000);
        info.details = VideoFile{.duration = std::chrono::milliseconds(lenght) };
    }
    else
        m_logger->warning(QString("Error when opening video file %1 for reading video details").arg(path));

    return info;
}
