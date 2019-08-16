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


struct ThumbnailGenerator::FromImageTask: ITaskExecutor::ITask
{
    FromImageTask(const QString& path,
                  int height,
                  std::unique_ptr<ICallback> callback,
                  const ThumbnailGenerator* generator):
        m_path(path),
        m_height(height),
        m_callback(std::move(callback)),
        m_generator(generator)
    {

    }

    virtual ~FromImageTask() {}

    FromImageTask(const FromImageTask &) = delete;
    FromImageTask& operator=(const FromImageTask &) = delete;

    virtual std::string name() const override
    {
        return "Image thumbnail generation";
    }

    virtual void perform() override
    {
        // TODO: use QTransform here to perform one transformation instead of many

        IExifReader* reader = m_generator->m_exifReaderFactory.get();

        Stopwatch stopwatch;
        stopwatch.start();

        QImage image;

        if(QFile::exists(m_path))
            image = Image::normalized(m_path, reader).get();

        if (image.isNull())
        {
            const QString error = QString("Broken image: %1").arg(m_path);

            m_generator->m_logger->error(error.toStdString());
        }

        const int photo_read = stopwatch.read(true);

        if (image.height() != m_height)
            image = image.scaledToHeight(m_height, Qt::SmoothTransformation);

        const int photo_scaling = stopwatch.stop();

        const std::string read_time_message = std::string("photo read time: ") + std::to_string(photo_read) + "ms";
        m_generator->m_logger->debug(read_time_message);

        const std::string scaling_time_message = std::string("photo scaling time: ") + std::to_string(photo_scaling) + "ms";
        m_generator->m_logger->debug(scaling_time_message);

        m_callback->result(image);
    }

    const QString m_path;
    const int m_height;
    const std::unique_ptr<ICallback> m_callback;
    const ThumbnailGenerator* m_generator;
};


struct ThumbnailGenerator::FromVideoTask: ITaskExecutor::ITask
{
    FromVideoTask(const QString& path,
                  int height,
                  std::unique_ptr<ICallback> callback,
                  const QString& ffmpeg,
                  const QString& ffprobe):
        m_path(path),
        m_height(height),
        m_callback(std::move(callback)),
        m_ffmpeg(ffmpeg),
        m_ffprobe(ffprobe)
    {

    }

    std::string name() const override
    {
        return "Video thumbnail generation";
    }

    void perform() override
    {
        const QFileInfo pathInfo(m_path);

        QImage result;

        if (pathInfo.exists())
        {
            const QString absolute_path = pathInfo.absoluteFilePath();

            const FFMpegVideoDetailsReader videoDetailsReader(m_ffprobe);
            const int seconds = videoDetailsReader.durationOf(absolute_path);
            auto tmpDir = System::getSysTmpDir("FromVideoTask");
            const QString thumbnail_path = System::getTmpFile(tmpDir->path(), "jpeg");

            QProcess ffmpeg_process4thumbnail;
            const QStringList ffmpeg_thumbnail_args =
            {
                "-y",                                        // overwrite file created with QTemporaryFile
                "-ss", QString::number(seconds / 10),
                "-i", absolute_path,
                "-vframes", "1",
                "-vf", QString("scale=-1:%1").arg(m_height),
                "-q:v", "2",
                thumbnail_path
            };

            ffmpeg_process4thumbnail.start(m_ffmpeg, ffmpeg_thumbnail_args );
            const bool status = ffmpeg_process4thumbnail.waitForFinished();

            if (status)
                result = QImage(thumbnail_path);
        }

        m_callback->result(result);
    }

    const QString m_path;
    const int m_height;
    const std::unique_ptr<ICallback> m_callback;
    const QString m_ffmpeg;
    const QString m_ffprobe;
};



ThumbnailGenerator::ThumbnailGenerator(ITaskExecutor* executor, ILogger* logger, IConfiguration* config):
    m_tasks(std::make_unique<TasksQueue>(executor)),
    m_logger(logger),
    m_configuration(config)
{

}


ThumbnailGenerator::~ThumbnailGenerator()
{

}


void ThumbnailGenerator::dismissPendingTasks()
{
    m_tasks->clear();
}


void ThumbnailGenerator::run(const QString& path, int height, std::unique_ptr<ICallback> callback)
{
    if (MediaTypes::isImageFile(path))
    {
        auto task = std::make_unique<FromImageTask>(path, height, std::move(callback), this);
        m_tasks->push(std::move(task));
    }
    else if (MediaTypes::isVideoFile(path))
    {
        const QVariant ffmpegVar = m_configuration->getEntry(ExternalToolsConfigKeys::ffmpegPath);
        const QString ffmpegPath = ffmpegVar.toString();
        const QVariant ffprobeVar = m_configuration->getEntry(ExternalToolsConfigKeys::ffprobePath);
        const QString ffprobePath = ffprobeVar.toString();
        const QFileInfo mpegfileInfo(ffmpegPath);
        const QFileInfo probefileInfo(ffprobePath);

        if (mpegfileInfo.isExecutable() && probefileInfo.isExecutable())
        {
            auto task = std::make_unique<FromVideoTask>(path, height, std::move(callback), ffmpegPath, ffprobePath);
            m_tasks->push(std::move(task));
        }
        else
            callback->result(QImage{});
    }
    else
        assert(!"unknown file type");
}
