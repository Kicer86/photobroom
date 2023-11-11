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

#include "thumbnail_generator.hpp"

#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <opencv2/opencv.hpp>

#include <system/system.hpp>
#include "constants.hpp"
#include "iconfiguration.hpp"
#include "iexif_reader.hpp"
#include "ilogger.hpp"
#include "image_tools.hpp"
#include "media_types.hpp"
#include "video_media_information.hpp"


ThumbnailGenerator::ThumbnailGenerator(ILogger* logger, IConfiguration* config):
    m_logger(logger),
    m_configuration(config)
{

}


ThumbnailGenerator::~ThumbnailGenerator()
{

}


QImage ThumbnailGenerator::generate(const QString& path, const ThumbnailParameters& params)
{
    const QImage frame = readFrame(path);
    QImage thumb;

    if (frame.isNull() == false)
        thumb = scaleImage(frame, params);

    return thumb;
}


QImage ThumbnailGenerator::generateFrom(const QImage& image, const ThumbnailParameters& params)
{
    return scaleImage(image, params);
}


QImage ThumbnailGenerator::readFrameFromImage(const QString& path) const
{
    IExifReader& reader = m_exifReaderFactory.get();

    QElapsedTimer stopwatch;
    stopwatch.start();

    QImage image;

    if(QFile::exists(path))
        image = Image::normalized(path, reader).get();

    if (image.isNull())
    {
        const QString error = QString("Broken image: %1").arg(path);

        m_logger->error(error);
    }

    const qint64 photo_read = stopwatch.elapsed();

    const QString read_time_message = QString("photo %1 read time: %2ms").arg(path).arg(photo_read);
    m_logger->debug(read_time_message);

    return image;
}


QImage ThumbnailGenerator::readFrameFromVideo(const QString& path) const
{
    const QFileInfo pathInfo(path);

    QImage result;

    if (pathInfo.exists())
    {
        m_logger->trace(QString("Opening video file %1 to read frame for thumbnail").arg(path));
        const QString absolute_path = pathInfo.absoluteFilePath();
        const VideoMediaInformation videoMediaInfo(*m_configuration, *m_logger);
        const auto fileInfo = videoMediaInfo.getInformation(absolute_path);

        if (std::holds_alternative<VideoFile>(fileInfo.details))
        {
            const auto videoInfo = std::get<VideoFile>(fileInfo.details);
            const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(videoInfo.duration).count();

            m_logger->trace(QString("Video file %1ms long").arg(milliseconds));

            if (milliseconds > 0)
            {
                cv::VideoCapture video(absolute_path.toStdString());

                if (video.isOpened())
                {
                    m_logger->trace("Stream opened successfully");
                    const double position = static_cast<double>(milliseconds) / 10.0;
                    video.set(cv::CAP_PROP_POS_MSEC, position);

                    cv::Mat frame;
                    video >> frame;

                    assert(frame.type() == CV_8UC3);

                    result = QImage(static_cast<uchar*>(frame.data), frame.cols, frame.rows, static_cast<qsizetype>(frame.step), QImage::Format_RGB888).rgbSwapped();
                }
                else
                    m_logger->warning(QString("Error while ppening video file %1 to read frame for thumbnail").arg(path));
            }
        }
    }

    m_logger->trace(QString("Video file %1 closed").arg(path));

    return result;
}


QImage ThumbnailGenerator::readFrame(const QString& path) const
{
    QImage image;

    if (MediaTypes::isImageFile(path))
        image = readFrameFromImage(path);
    else if (MediaTypes::isVideoFile(path))
        image = readFrameFromVideo(path);
    else
        m_logger->error(QString("Unknown file type: %1").arg(path));

    return image;
}


QImage ThumbnailGenerator::scaleImage(const QImage& image, const ThumbnailParameters& params) const
{
    QImage thumbnail;

    const QSize& size = std::get<0>(params);

    QElapsedTimer stopwatch;
    stopwatch.start();

    if (image.width() < image.height())
        thumbnail = image.scaledToWidth(size.width(), Qt::SmoothTransformation);
    else
        thumbnail = image.scaledToHeight(size.height(), Qt::SmoothTransformation);

    const qint64 photo_scaling = stopwatch.elapsed();

    const QString scaling_time_message = QString("photo scaling time: %1ms").arg(photo_scaling);
    m_logger->debug(scaling_time_message);

    return thumbnail;
}
