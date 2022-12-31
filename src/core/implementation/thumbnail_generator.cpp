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
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QProcess>
#include <QVideoFrame>
#include <QVideoSink>

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
        const QString absolute_path = pathInfo.absoluteFilePath();
        const VideoMediaInformation videoMediaInfo(*m_configuration);
        const auto fileInfo = videoMediaInfo.getInformation(absolute_path);
        const auto videoInfo = std::get<VideoFile>(fileInfo.details);
        const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(videoInfo.duration).count();

        if (milliseconds > 0)
        {
            QVideoSink videoSink;
            QMediaPlayer player;

            // load media and wait for finish
            {
                QEventLoop eventLoop;
                QObject::connect(&player, &QMediaPlayer::mediaStatusChanged, &eventLoop, [&eventLoop]{ eventLoop.exit(); });
                player.setVideoSink(&videoSink);
                player.setSource(QUrl::fromLocalFile(path));
                eventLoop.exec();
            }

            player.play();

            // set desired position
            if (player.isSeekable())
            {
                QEventLoop eventLoop;
                QObject::connect(&player, &QMediaPlayer::positionChanged, &eventLoop, [&eventLoop]{ eventLoop.exit(); });
                QObject::connect(&player, &QMediaPlayer::mediaStatusChanged, &eventLoop, [&eventLoop]{ eventLoop.exit(); });
                player.setPosition(milliseconds / 10);
                eventLoop.exec();
            }

            // wait for frame to be ready
            if (player.error() == QMediaPlayer::NoError && player.playbackState() == QMediaPlayer::PlayingState)
            {
                QEventLoop eventLoop;
                QObject::connect(&videoSink, &QVideoSink::videoFrameChanged, &eventLoop, [&eventLoop]{ eventLoop.exit(); });
                eventLoop.exec();

                player.stop();
                const auto frame = videoSink.videoFrame();

                result = frame.toImage();
            }
        }
    }

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
