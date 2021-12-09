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
#include <QProcess>

#include <system/system.hpp>
#include "constants.hpp"
#include "exiftool_video_details_reader.hpp"
#include "iconfiguration.hpp"
#include "iexif_reader.hpp"
#include "ilogger.hpp"
#include "image_tools.hpp"
#include "media_types.hpp"


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


QImage ThumbnailGenerator::readFrameFromVideo(const QString& path, const QString& exiftool, const QString& ffmpeg) const
{
    const QFileInfo pathInfo(path);

    QImage result;

    if (pathInfo.exists())
    {
        const QString absolute_path = pathInfo.absoluteFilePath();
        const auto output = ExiftoolUtils::exiftoolOutput(exiftool, absolute_path);
        const auto entries = ExiftoolUtils::parseOutput(output);
        const ExiftoolVideoDetailsReader videoDetailsReader(entries);
        const std::optional<int> seconds = videoDetailsReader.durationOf();

        if (seconds)
        {
            auto tmpDir = System::createTmpDir("FromVideoTask", System::Confidential);
            const QString thumbnail_path = System::getUniqueFileName(tmpDir->path(), "jpeg");

            QProcess ffmpeg_process4thumbnail;
            const QStringList ffmpeg_thumbnail_args =
            {
                "-y",                                        // overwrite file created with QTemporaryFile
                "-ss", QString::number(*seconds / 10),
                "-i", absolute_path,
                "-vframes", "1",
                "-q:v", "2",
                thumbnail_path
            };

            ffmpeg_process4thumbnail.start(ffmpeg, ffmpeg_thumbnail_args );
            const bool status = ffmpeg_process4thumbnail.waitForFinished();

            if (status)
                result = QImage(thumbnail_path);
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
    {
        const QVariant ffmpegVar = m_configuration->getEntry(ExternalToolsConfigKeys::ffmpegPath);
        const QString ffmpegPath = ffmpegVar.toString();
        const QVariant exiftoolVar = m_configuration->getEntry(ExternalToolsConfigKeys::exiftoolPath);
        const QString exiftoolPath = exiftoolVar.toString();
        const QFileInfo mpegfileInfo(ffmpegPath);
        const QFileInfo exiftoolFileInfo(exiftoolPath);

        if (mpegfileInfo.isExecutable() && exiftoolFileInfo.isExecutable())
            image = readFrameFromVideo(path, exiftoolPath, ffmpegPath);
    }
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
