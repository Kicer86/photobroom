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

#include <cassert>
#include <QTimeZone>

#include "constants.hpp"
#include "video_media_information.hpp"


namespace
{
    AVStream* findVideoStream(AVFormatContext* context)
    {
        for (auto i = 0u; i < context->nb_streams; ++i)
            if (context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                return context->streams[i];

        return nullptr;
    }

    QDateTime getCreationTime(AVFormatContext* context)
    {
        const auto creationTimeEntry = context->metadata? av_dict_get(context->metadata, "creation_time", nullptr, 0) : nullptr;
        const QString creationTimeStr = creationTimeEntry? creationTimeEntry->value: nullptr;

        if (creationTimeStr.isEmpty() == false)
        {
            const auto dateTimeSplit = creationTimeStr.split('T');
            assert(dateTimeSplit.size() == 2);
            const auto date = dateTimeSplit.first();
            const auto time = dateTimeSplit.last().left(8);         // left() for removing any leftovers

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


FileInformation VideoMediaInformation::getInformation(const QString& path) const
{
    FileInformation info;
    VideoFile videoInfo;

    IExifReader& exif = m_exif.get();

    const auto exif_creation_time = exif.get(path, IExifReader::TagType::Xmp_video_DateTimeOriginal);
    auto width = exif.get(path, IExifReader::TagType::Xmp_video_Width);
    auto height = exif.get(path, IExifReader::TagType::Xmp_video_Height);

    if (exif_creation_time)
    {
        const auto creation_time = std::any_cast<std::string>(*exif_creation_time);
        const auto creation_time_qstr = QString::fromStdString(creation_time);

        info.common.creationTime = QDateTime::fromString(creation_time_qstr, "yyyy:MM:dd hh:mm:ss");
    }

    AVFormatContext* formatContext = nullptr;
    if (avformat_open_input(&formatContext, path.toStdString().c_str(), NULL, NULL) == 0)
    {
        if (avformat_find_stream_info(formatContext, NULL) >= 0)
        {
            if (info.common.creationTime.has_value() == false)
                info.common.creationTime = getCreationTime(formatContext);

            auto videoStream = findVideoStream(formatContext);

            double rotation = 0.0;
            for (int i = 0; videoStream && i < videoStream->nb_side_data; ++i) {
                AVPacketSideData *sideData = &videoStream->side_data[i];
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

    avformat_close_input(&formatContext);
    avformat_free_context(formatContext);

    info.details = videoInfo;

    return info;
}
