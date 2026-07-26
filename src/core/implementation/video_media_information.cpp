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

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/display.h>
}

#include <iostream>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <span>

#include <QTimeZone>

#include "constants.hpp"
#include "video_media_information.hpp"


namespace
{
    class FileAvio
    {
    public:
        explicit FileAvio(const Filesystem::IFile& file)
            : m_data(file.byteView())
        {
            m_formatContext = avformat_alloc_context();
            if (m_formatContext == nullptr)
                return;

            constexpr int bufferSize = 4096;
            auto* buffer = static_cast<unsigned char*>(av_malloc(bufferSize));
            if (buffer == nullptr)
                return;

            m_ioContext = avio_alloc_context(
                buffer,
                bufferSize,
                0,
                this,
                &FileAvio::read,
                nullptr,
                &FileAvio::seek
            );

            if (m_ioContext == nullptr)
            {
                av_free(buffer);
                return;
            }

            m_formatContext->pb = m_ioContext;
            m_formatContext->flags |= AVFMT_FLAG_CUSTOM_IO;
        }

        FileAvio(const FileAvio&) = delete;
        FileAvio& operator=(const FileAvio&) = delete;

        ~FileAvio()
        {
            if (m_formatContext != nullptr)
                avformat_close_input(&m_formatContext);

            if (m_ioContext != nullptr)
            {
                av_freep(&m_ioContext->buffer);
                avio_context_free(&m_ioContext);
            }
        }

        AVFormatContext* open()
        {
            if (m_formatContext == nullptr || m_ioContext == nullptr)
                return nullptr;

            if (avformat_open_input(&m_formatContext, nullptr, nullptr, nullptr) < 0)
                return nullptr;

            return m_formatContext;
        }

    private:
        static int read(void* opaque, unsigned char* buffer, int bufferSize)
        {
            return static_cast<FileAvio*>(opaque)->readImpl(buffer, bufferSize);
        }

        int readImpl(unsigned char* buffer, int bufferSize)
        {
            if (m_position >= m_data.size())
                return AVERROR_EOF;

            const auto remaining = m_data.size() - m_position;
            const auto amount = std::min<std::size_t>(remaining, static_cast<std::size_t>(bufferSize));
            std::copy_n(m_data.begin() + m_position, amount, buffer);
            m_position += amount;

            return static_cast<int>(amount);
        }

        static int64_t seek(void* opaque, int64_t offset, int whence)
        {
            return static_cast<FileAvio*>(opaque)->seekImpl(offset, whence);
        }

        int64_t seekImpl(int64_t offset, int whence)
        {
            if ((whence & AVSEEK_SIZE) != 0)
            {
                if (m_data.size() > static_cast<std::size_t>(std::numeric_limits<int64_t>::max()))
                    return AVERROR(EINVAL);

                return static_cast<int64_t>(m_data.size());
            }

            whence &= ~AVSEEK_FORCE;

            int64_t base = 0;
            switch (whence)
            {
                case SEEK_SET:
                    base = 0;
                    break;
                case SEEK_CUR:
                    if (m_position > static_cast<std::size_t>(std::numeric_limits<int64_t>::max()))
                        return AVERROR(EINVAL);
                    base = static_cast<int64_t>(m_position);
                    break;
                case SEEK_END:
                    if (m_data.size() > static_cast<std::size_t>(std::numeric_limits<int64_t>::max()))
                        return AVERROR(EINVAL);
                    base = static_cast<int64_t>(m_data.size());
                    break;
                default:
                    return AVERROR(EINVAL);
            }

            if ((offset > 0 && base > std::numeric_limits<int64_t>::max() - offset) ||
                (offset < 0 && base < std::numeric_limits<int64_t>::min() - offset))
                return AVERROR(EINVAL);

            const int64_t position = base + offset;
            if (position < 0 || static_cast<uint64_t>(position) > m_data.size())
                return AVERROR(EINVAL);

            m_position = static_cast<std::size_t>(position);
            return position;
        }

        std::span<const std::uint8_t> m_data;
        std::size_t m_position = 0;
        AVFormatContext* m_formatContext = nullptr;
        AVIOContext* m_ioContext = nullptr;
    };

    AVStream* findVideoStream(AVFormatContext* context)
    {
        for (auto i = 0u; i < context->nb_streams; ++i)
            if (context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                return context->streams[i];

        return nullptr;
    }

    std::optional<QDateTime> getCreationTime(AVFormatContext* context)
    {
        const auto creationTimeEntry = context->metadata? av_dict_get(context->metadata, "creation_time", nullptr, 0) : nullptr;
        QString creationTimeStr = creationTimeEntry? creationTimeEntry->value: nullptr;

        if (creationTimeStr.isEmpty() == false)
        {
            //                             (date + space + time)
            assert(creationTimeStr.length() >= (10 + 1 + 8) && (creationTimeStr[10] == 'T' || creationTimeStr[10] == ' '));
            const auto date = creationTimeStr.left(10);
            const auto time = creationTimeStr.mid(11, 8);
            const auto dateQ = QDate::fromString(date, "yyyy-MM-dd");
            const auto timeQ = QTime::fromString(time, "hh:mm:ss");

            QDateTime creationTime(dateQ, timeQ);

            return creationTime;
        }
        else
            return {};
    }
}


VideoMediaInformation::VideoMediaInformation(IExifReaderFactory& exif, const ILogger& logger)
    : m_logger(logger.subLogger("VideoMediaInformation"))
    , m_exif(exif)
{

}


FileInformation VideoMediaInformation::getInformation(const Filesystem::IFile& file) const
{
    FileInformation info;
    VideoFile videoInfo;

    IExifReader& exif = m_exif.get();
    const QString path = QString::fromStdString(file.path());

    const auto exif_creation_time = exif.get(path, IExifReader::TagType::Xmp_video_DateTimeOriginal);
    auto width = exif.get(path, IExifReader::TagType::Xmp_video_Width);
    auto height = exif.get(path, IExifReader::TagType::Xmp_video_Height);

    if (exif_creation_time)
    {
        const auto creation_time = std::any_cast<std::string>(*exif_creation_time);
        const auto creation_time_qstr = QString::fromStdString(creation_time);

        info.common.creationTime = QDateTime::fromString(creation_time_qstr, "yyyy:MM:dd hh:mm:ss");
    }

    FileAvio fileAvio(file);
    AVFormatContext* formatContext = fileAvio.open();
    if (formatContext != nullptr)
    {
        if (avformat_find_stream_info(formatContext, NULL) >= 0)
        {
            if (info.common.creationTime.has_value() == false)
                info.common.creationTime = getCreationTime(formatContext);

            auto videoStream = findVideoStream(formatContext);

            double rotation = 0.0;
            for (int i = 0; videoStream && i < videoStream->codecpar->nb_coded_side_data; ++i) {
                AVPacketSideData *sideData = &videoStream->codecpar->coded_side_data[i];
                if (sideData->type == AV_PKT_DATA_DISPLAYMATRIX)
                {
                    rotation = av_display_rotation_get(reinterpret_cast<int32_t*>(sideData->data));

                    break;
                }
            }

            const int rotationInt = static_cast<int>(rotation);
            if (rotationInt == 90 || rotationInt == -90)
                std::swap(width, height);

            static_assert(AV_TIME_BASE / 1000 >= 1, "AV_TIME_BASE not big enought to provide millisecond resolution");
            static_assert(AV_TIME_BASE % 1000 == 0, "AV_TIME_BASE is not suitable for millisecond resolution");
            videoInfo.duration = std::chrono::milliseconds(formatContext->duration / (AV_TIME_BASE / 1000));
        }
    }

    if (width && height)
        info.common.dimension = QSize(std::any_cast<int>(*width), std::any_cast<int>(*height));

    info.details = videoInfo;

    return info;
}
