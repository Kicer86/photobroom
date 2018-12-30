/*
 * Thumbnail generator.
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "thumbnail_generator.hpp"

#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTemporaryFile>

#include <core/constants.hpp>
#include <core/ffmpeg_video_details_reader.hpp>
#include <core/iconfiguration.hpp>
#include <core/iexif_reader.hpp>
#include <core/image_tools.hpp>
#include <core/itask_executor.hpp>
#include <core/ilogger.hpp>
#include <core/media_types.hpp>
#include <core/stopwatch.hpp>
#include <system/system.hpp>

#include "images/images.hpp"


struct ThumbnailGenerator::FromImageTask: ITaskExecutor::ITask
{
    FromImageTask(const ThumbnailInfo& info,
                  const ThumbnailGenerator::Callback& callback,
                  const ThumbnailGenerator* generator):
        m_info(info),
        m_callback(callback),
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

        IExifReader* reader = m_generator->m_exifReaderFactory->get();

        Stopwatch stopwatch;
        stopwatch.start();

        QImage image = QFile::exists(m_info.path)?
            Image::normalized(m_info.path, reader):
            QImage(Images::error);

        if (image.isNull())
        {
            const QString error = QString("Broken image: %1").arg(m_info.path);

            m_generator->m_logger->error(error.toStdString());
            image = QImage(Images::error);
        }

        const int photo_read = stopwatch.read(true);

        if (image.height() != m_info.height)
            image = image.scaledToHeight(m_info.height, Qt::SmoothTransformation);

        const int photo_scaling = stopwatch.stop();

        const std::string read_time_message = std::string("photo read time: ") + std::to_string(photo_read) + "ms";
        m_generator->m_logger->debug(read_time_message);

        const std::string scaling_time_message = std::string("photo scaling time: ") + std::to_string(photo_scaling) + "ms";
        m_generator->m_logger->debug(scaling_time_message);

        m_callback(m_info, image);
    }

    ThumbnailInfo m_info;
    ThumbnailGenerator::Callback m_callback;
    const ThumbnailGenerator* m_generator;
};


struct ThumbnailGenerator::FromVideoTask: ITaskExecutor::ITask
{
    FromVideoTask(const ThumbnailInfo& info,
                  const ThumbnailGenerator::Callback& callback,
                  const QString& ffprobe):
        m_thumbnailInfo(info),
        m_callback(callback),
        m_ffprobe(ffprobe)
    {

    }

    std::string name() const override
    {
        return "Video thumbnail generation";
    }

    void perform() override
    {
        const QFileInfo pathInfo(m_thumbnailInfo.path);
        const QString absolute_path = pathInfo.absoluteFilePath();

        const FFMpegVideoDetailsReader videoDetailsReader(m_ffprobe);
        const int seconds = videoDetailsReader.durationOf(absolute_path);
        const QString thumbnail_path_pattern = System::getTempFilePatternFor("jpeg");
        QTemporaryFile thumbnail(thumbnail_path_pattern);
        thumbnail.open();                                // create file

        const QString thumbnail_path = thumbnail.fileName();
        thumbnail.close();                               // close but do not delete file. Just make sure it is not locked

        QProcess ffmpeg_process4thumbnail;
        const QStringList ffmpeg_thumbnail_args =
        {
            "-y",                                        // overwrite file created with QTemporaryFile
            "-ss", QString::number(seconds / 10),
            "-i", absolute_path,
            "-vframes", "1",
            "-vf", QString("scale=-1:%1").arg(m_thumbnailInfo.height),
            "-q:v", "2",
            thumbnail_path
        };

        ffmpeg_process4thumbnail.start("ffmpeg", ffmpeg_thumbnail_args );
        const bool status = ffmpeg_process4thumbnail.waitForFinished();

        if (status)
        {
            const QImage thumbnail_image(thumbnail_path);

            m_callback(m_thumbnailInfo, thumbnail_image);
        }
    }

    const ThumbnailInfo m_thumbnailInfo;
    const ThumbnailGenerator::Callback m_callback;
    const QString m_ffprobe;
};


uint qHash(const ThumbnailInfo& key, uint seed = 0)
{
    return qHash(key.path) ^ qHash(key.height) ^ seed;
}


ThumbnailGenerator::ThumbnailGenerator():
    m_videoImage(":/gui/video.svg"),
    m_tasks(),
    m_logger(nullptr),
    m_exifReaderFactory(nullptr),
    m_configuration(nullptr)
{

}


ThumbnailGenerator::~ThumbnailGenerator()
{

}


void ThumbnailGenerator::dismissPendingTasks()
{
    m_tasks->clear();
}


void ThumbnailGenerator::set(ITaskExecutor* executor)
{
    m_tasks = std::make_unique<TasksQueue>(executor);
}


void ThumbnailGenerator::set(ILogger* logger)
{
    m_logger = logger;
}


void ThumbnailGenerator::set(IExifReaderFactory* exifFactory)
{
    m_exifReaderFactory = exifFactory;
}


void ThumbnailGenerator::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}


void ThumbnailGenerator::generateThumbnail(const ThumbnailInfo& info, const Callback& callback) const
{
    const QString& path = info.path;

    if (MediaTypes::isImageFile(path))
    {
        auto task = std::make_unique<FromImageTask>(info, callback, this);
        m_tasks->push(std::move(task));
    }
    else if (MediaTypes::isVideoFile(path))
    {
        const QVariant ffprobeVar = m_configuration->getEntry(ExternalToolsConfigKeys::ffprobePath);
        const QString ffprobePath = ffprobeVar.toString();
        const QFileInfo fileInfo(ffprobePath);

        if (fileInfo.isExecutable())
        {
            auto task = std::make_unique<FromVideoTask>(info, callback, ffprobePath);
            m_tasks->push(std::move(task));
        }
        else
            callback(info, m_videoImage);
    }
    else
        assert(!"unknown file type");
}


ThumbnailCache::ThumbnailCache():
    m_cacheMutex(),
    m_cache(2048)
{

}


ThumbnailCache::~ThumbnailCache()
{

}


void ThumbnailCache::add(const ThumbnailInfo& info, const QImage& img)
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);

    QImage* new_img = new QImage(img);

    m_cache.insert(info, new_img);
}


std::optional<QImage> ThumbnailCache::get(const ThumbnailInfo& info) const
{
    std::optional<QImage> result;

    std::lock_guard<std::mutex> lock(m_cacheMutex);

    if (m_cache.contains(info))
        result = *m_cache[info];

    return result;
}


void ThumbnailCache::clear()
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    m_cache.clear();
}
