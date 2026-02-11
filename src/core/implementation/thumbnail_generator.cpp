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

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/display.h>
#include <libavutil/error.h>
#include <libavutil/mathematics.h>
#include <libavutil/rational.h>
#include <libswscale/swscale.h>
}

#include "constants.hpp"
#include "iconfiguration.hpp"
#include "iexif_reader.hpp"
#include "ilogger.hpp"
#include "image_tools.hpp"
#include "media_types.hpp"
#include "video_media_information.hpp"

import broom.system;

namespace
{
    struct FormatContextGuard
    {
        AVFormatContext* context = nullptr;

        ~FormatContextGuard()
        {
            if (context != nullptr)
                avformat_close_input(&context);
        }
    };

    struct CodecContextGuard
    {
        AVCodecContext* context = nullptr;

        ~CodecContextGuard()
        {
            if (context != nullptr)
                avcodec_free_context(&context);
        }
    };

    struct PacketGuard
    {
        AVPacket* packet = av_packet_alloc();

        ~PacketGuard()
        {
            if (packet != nullptr)
                av_packet_free(&packet);
        }
    };

    struct FrameGuard
    {
        AVFrame* frame = av_frame_alloc();

        ~FrameGuard()
        {
            if (frame != nullptr)
                av_frame_free(&frame);
        }
    };

    struct SwsContextGuard
    {
        SwsContext* context = nullptr;

        ~SwsContextGuard()
        {
            if (context != nullptr)
                sws_freeContext(context);
        }
    };

    bool seekToPosition(AVFormatContext* formatContext, AVCodecContext* codecContext, AVStream* stream, int streamIndex, int64_t positionMs)
    {
        if (formatContext == nullptr || codecContext == nullptr || stream == nullptr)
            return false;

        const AVRational millisecondsBase{1, 1000};
        const int64_t targetTimestamp = av_rescale_q(positionMs, millisecondsBase, stream->time_base);

        if (av_seek_frame(formatContext, streamIndex, targetTimestamp, AVSEEK_FLAG_BACKWARD) >= 0)
        {
            avcodec_flush_buffers(codecContext);
            return true;
        }

        return false;
    }

    bool decodeFrame(AVFormatContext* formatContext, AVCodecContext* codecContext, int streamIndex, AVFrame* frame)
    {
        PacketGuard packet;

        if (formatContext == nullptr || codecContext == nullptr || frame == nullptr || packet.packet == nullptr)
            return false;

        while (av_read_frame(formatContext, packet.packet) >= 0)
        {
            if (packet.packet->stream_index != streamIndex)
            {
                av_packet_unref(packet.packet);
                continue;
            }

            const int sendResult = avcodec_send_packet(codecContext, packet.packet);
            av_packet_unref(packet.packet);

            if (sendResult < 0)
                continue;

            while (true)
            {
                const int receiveResult = avcodec_receive_frame(codecContext, frame);

                if (receiveResult == AVERROR(EAGAIN) || receiveResult == AVERROR_EOF)
                    break;

                if (receiveResult < 0)
                    return false;

                return true;
            }
        }

        return false;
    }

    QImage frameToQImage(const AVFrame* frame)
    {
        if (frame == nullptr || frame->width <= 0 || frame->height <= 0)
            return {};

        QImage image(frame->width, frame->height, QImage::Format_RGB888);

        if (image.isNull())
            return {};

        SwsContextGuard swsContext;
        swsContext.context = sws_getContext(
            frame->width,
            frame->height,
            static_cast<AVPixelFormat>(frame->format),
            frame->width,
            frame->height,
            AV_PIX_FMT_RGB24,
            SWS_BILINEAR,
            nullptr,
            nullptr,
            nullptr);

        if (swsContext.context == nullptr)
            return {};

        uint8_t* const dstData[4] = { image.bits(), nullptr, nullptr, nullptr };
        const int dstLinesize[4] = { static_cast<int>(image.bytesPerLine()), 0, 0, 0 };

        sws_scale(swsContext.context, frame->data, frame->linesize, 0, frame->height, dstData, dstLinesize);

        return image;
    }

    QImage readVideoFrame(const QString& absolutePath, const QString& logPath, int64_t positionMs, ILogger* logger)
    {
        FormatContextGuard formatContext;
        const int openResult = avformat_open_input(&formatContext.context, absolutePath.toStdString().c_str(), nullptr, nullptr);

        if (openResult != 0)
        {
            logger->warning(QString("Error while opening video file %1 to read frame for thumbnail").arg(logPath));
            return {};
        }

        if (avformat_find_stream_info(formatContext.context, nullptr) < 0)
        {
            logger->warning(QString("Error while reading stream info for %1").arg(logPath));
            return {};
        }

        const int videoStreamIndex = av_find_best_stream(formatContext.context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

        if (videoStreamIndex < 0)
        {
            logger->warning(QString("No video stream found in %1").arg(logPath));
            return {};
        }

        AVStream* const stream = formatContext.context->streams[videoStreamIndex];
        const AVCodec* const codec = avcodec_find_decoder(stream->codecpar->codec_id);

        if (codec == nullptr)
        {
            logger->warning(QString("No decoder found for video file %1").arg(logPath));
            return {};
        }

        CodecContextGuard codecContext;
        codecContext.context = avcodec_alloc_context3(codec);

        if (codecContext.context == nullptr)
            return {};

        const bool opened = avcodec_parameters_to_context(codecContext.context, stream->codecpar) == 0 &&
            avcodec_open2(codecContext.context, codec, nullptr) == 0;

        if (!opened)
        {
            logger->warning(QString("Error while opening video stream %1 to read frame for thumbnail").arg(logPath));
            return {};
        }

        logger->trace("Stream opened successfully");
        seekToPosition(formatContext.context, codecContext.context, stream, videoStreamIndex, positionMs);

        FrameGuard frame;
        if (frame.frame == nullptr)
            return {};

        if (!decodeFrame(formatContext.context, codecContext.context, videoStreamIndex, frame.frame))
            return {};

        return frameToQImage(frame.frame);
    }
}


ThumbnailGenerator::ThumbnailGenerator(ILogger* logger, IExifReaderFactory& exif):
    m_logger(logger),
    m_exif(exif)
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
        const QString absolutePath = pathInfo.absoluteFilePath();
        const VideoMediaInformation videoMediaInfo(m_exif, *m_logger);
        const auto fileInfo = videoMediaInfo.getInformation(absolutePath);

        if (std::holds_alternative<VideoFile>(fileInfo.details))
        {
            const auto videoInfo = std::get<VideoFile>(fileInfo.details);
            const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(videoInfo.duration).count();

            m_logger->trace(QString("Video file %1ms long").arg(milliseconds));

            if (milliseconds > 0)
            {
                const int64_t positionMs = milliseconds / 10;
                result = readVideoFrame(absolutePath, path, positionMs, m_logger);
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
