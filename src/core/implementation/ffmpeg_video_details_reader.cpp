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

#include "ffmpeg_video_details_reader.hpp"

#include <cassert>

#include <QProcess>
#include <QRegExp>
#include <QTime>


FFMpegVideoDetailsReader::FFMpegVideoDetailsReader(const QString& ffmpeg): m_ffprobePath(ffmpeg)
{
    assert(ffmpeg.isEmpty() == false);
}


bool FFMpegVideoDetailsReader::hasDetails(const QString& filePath) const
{
    QProcess ffprobe_process;
    ffprobe_process.setProcessChannelMode(QProcess::MergedChannels);

    const QStringList ffprobe_args = { filePath };

    ffprobe_process.start(m_ffprobePath, ffprobe_args );
    const bool status = ffprobe_process.waitForFinished() &&
                        ffprobe_process.exitCode() == 0;

    return status;
}



std::optional<QSize> FFMpegVideoDetailsReader::resolutionOf(const QString& video_file) const
{
    const QStringList output = outputFor(video_file);

    QRegExp resolution_regex(".*Stream [^ ]+ Video:.*, ([0-9]+)x([0-9]+).*");

    std::optional<QSize> result;
    for(const QString& line: output)
    {
        const bool matched = resolution_regex.exactMatch(line);

        if (matched)
        {
            const QStringList captured = resolution_regex.capturedTexts();
            const QString resolution_x_str = captured[1];
            const QString resolution_y_str = captured[2];

            const int resolution_x = resolution_x_str.toInt();
            const int resolution_y = resolution_y_str.toInt();

            result = QSize(resolution_x, resolution_y);

            const int r = rotation(output);

            if (r == 90)
                result->transpose();

            break;
        }
    }

    return result;
}


int FFMpegVideoDetailsReader::durationOf(const QString& video_file) const
{
    const QStringList output = outputFor(video_file);

    QRegExp duration_regex(".*Duration: ([0-9:\\.]+).*");

    int duration = -1;

    for(const QString& line: output)
    {
        const bool matched = duration_regex.exactMatch(line);

        if (matched)
        {
            const QStringList captured = duration_regex.capturedTexts();
            const QString duration_str = captured[1] + "0";                 // convert 100th parts of second to miliseconds
            const QTime duration_time = QTime::fromString(duration_str, "hh:mm:ss.zzz");
            duration = QTime(0, 0, 0).secsTo(duration_time);

            break;
        }
    }

    return duration;
}


QStringList FFMpegVideoDetailsReader::outputFor(const QString& video_file) const
{
    QProcess ffprobe_process;
    ffprobe_process.setProcessChannelMode(QProcess::MergedChannels);

    const QStringList ffprobe_args = { video_file };

    ffprobe_process.start(m_ffprobePath, ffprobe_args );
    bool status = ffprobe_process.waitForFinished();

    QStringList result;
    if (status)
        while(ffprobe_process.canReadLine())
        {
            const QByteArray line = ffprobe_process.readLine();
            result.append(line.constData());
        }

    return result;
}


int FFMpegVideoDetailsReader::rotation(const QStringList& output) const
{
    QRegExp rotation_regex(" *rotate *: ([0-9]+).*");
    int rotation = 0;

    for(const QString& line: output)
    {
        const bool matched = rotation_regex.exactMatch(line);

        if (matched)
        {
            const QStringList captured = rotation_regex.capturedTexts();
            const QString rotation_str = captured[1];

            rotation = rotation_str.toInt();

            break;
        }
    }

    return rotation;
}
