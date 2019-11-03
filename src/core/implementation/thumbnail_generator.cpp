/*
 * Tool for generating thumbnails
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

#include <QFile>
#include <QFileInfo>
#include <QProcess>

#include <system/system.hpp>
#include "constants.hpp"
#include "ffmpeg_video_details_reader.hpp"
#include "iconfiguration.hpp"
#include "iexif_reader.hpp"
#include "ilogger.hpp"
#include "image_tools.hpp"
#include "media_types.hpp"
#include "stopwatch.hpp"


ThumbnailGenerator::ThumbnailGenerator(ILogger* logger, IConfiguration* config):
    m_logger(logger),
    m_configuration(config)
{

}


ThumbnailGenerator::~ThumbnailGenerator()
{

}


QImage ThumbnailGenerator::generate(const QString& path, int height)
{
    QImage image;

    if (MediaTypes::isImageFile(path))
        image = fromImage(path, height);
    else if (MediaTypes::isVideoFile(path))
    {
        const QVariant ffmpegVar = m_configuration->getEntry(ExternalToolsConfigKeys::ffmpegPath);
        const QString ffmpegPath = ffmpegVar.toString();
        const QVariant ffprobeVar = m_configuration->getEntry(ExternalToolsConfigKeys::ffprobePath);
        const QString ffprobePath = ffprobeVar.toString();
        const QFileInfo mpegfileInfo(ffmpegPath);
        const QFileInfo probefileInfo(ffprobePath);

        if (mpegfileInfo.isExecutable() && probefileInfo.isExecutable())
            image = fromVideo(path, height, ffprobePath, ffmpegPath);
    }
    else
        assert(!"unknown file type");

    return image;
}


QImage ThumbnailGenerator::fromImage(const QString& path, int height)
{
    // TODO: use QTransform here to perform one transformation instead of many

    IExifReader* reader = m_exifReaderFactory.get();

    Stopwatch stopwatch;
    stopwatch.start();

    QImage image;

    if(QFile::exists(path))
        image = Image::normalized(path, reader).get();

    if (image.isNull())
    {
        const QString error = QString("Broken image: %1").arg(path);

        m_logger->error(error.toStdString());
    }

    const int photo_read = stopwatch.read(true);

    if (image.isNull() == false && image.height() != height)
        image = image.scaledToHeight(height, Qt::SmoothTransformation);

    const int photo_scaling = stopwatch.stop();

    const QString read_time_message = QString("photo %1 read time: %2ms").arg(path).arg(photo_read);
    m_logger->debug(read_time_message.toStdString());

    const std::string scaling_time_message = std::string("photo scaling time: ") + std::to_string(photo_scaling) + "ms";
    m_logger->debug(scaling_time_message);

    return image;
}


QImage ThumbnailGenerator::fromVideo(const QString& path, int height, const QString& ffprobe, const QString& ffmpeg)
{
    const QFileInfo pathInfo(path);

    QImage result;

    if (pathInfo.exists())
    {
        const QString absolute_path = pathInfo.absoluteFilePath();

        const FFMpegVideoDetailsReader videoDetailsReader(ffprobe);
        const int seconds = videoDetailsReader.durationOf(absolute_path);
        auto tmpDir = System::createTmpDir("FromVideoTask", System::Confidential);
        const QString thumbnail_path = System::getTmpFile(tmpDir->path(), "jpeg");

        QProcess ffmpeg_process4thumbnail;
        const QStringList ffmpeg_thumbnail_args =
        {
            "-y",                                        // overwrite file created with QTemporaryFile
            "-ss", QString::number(seconds / 10),
            "-i", absolute_path,
            "-vframes", "1",
            "-vf", QString("scale=-1:%1").arg(height),
            "-q:v", "2",
            thumbnail_path
        };

        ffmpeg_process4thumbnail.start(ffmpeg, ffmpeg_thumbnail_args );
        const bool status = ffmpeg_process4thumbnail.waitForFinished();

        if (status)
            result = QImage(thumbnail_path);
    }

    return result;
}
