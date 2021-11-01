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
#include <QRegularExpression>
#include <QTime>


FFMpegVideoDetailsReader::FFMpegVideoDetailsReader(const QString& ffmpeg, const QString& path)
    : m_ffprobePath(ffmpeg)
    , m_output(outputFor(path))
{
    assert(ffmpeg.isEmpty() == false);
}


bool FFMpegVideoDetailsReader::hasDetails() const
{
    return m_output.empty() == false;
}


std::optional<QSize> FFMpegVideoDetailsReader::resolutionOf() const
{
    QRegularExpression resolution_regex(".*Stream [^ ]+ Video:.*, ([0-9]+)x([0-9]+).*");

    std::optional<QSize> result;
    for(const QString& line: m_output)
    {
        const auto match = resolution_regex.match(line);

        if (match.hasMatch())
        {
            const QStringList captured = match.capturedTexts();
            const QString resolution_x_str = captured[1];
            const QString resolution_y_str = captured[2];

            const int resolution_x = resolution_x_str.toInt();
            const int resolution_y = resolution_y_str.toInt();

            result = QSize(resolution_x, resolution_y);

            const int r = rotation();

            if (r == 90)
                result->transpose();

            break;
        }
    }

    return result;
}


int FFMpegVideoDetailsReader::durationOf() const
{
    QRegularExpression duration_regex(".*Duration: ([0-9:\\.]+).*");

    int duration = -1;

    for(const QString& line: m_output)
    {
        const auto match = duration_regex.match(line);

        if (match.hasMatch())
        {
            const QStringList captured = match.capturedTexts();
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
    status &= ffprobe_process.exitCode() == 0;

    QStringList result;
    if (status)
        while(ffprobe_process.canReadLine())
        {
            const QByteArray line = ffprobe_process.readLine();
            result.append(line.constData());
        }

    return result;
}


int FFMpegVideoDetailsReader::rotation() const
{
    QRegularExpression rotation_regex(" *rotate *: ([0-9]+).*");
    int rotation = 0;

    for(const QString& line: m_output)
    {
        const auto match = rotation_regex.match(line);

        if (match.hasMatch())
        {
            const QStringList captured = match.capturedTexts();
            const QString rotation_str = captured[1];

            rotation = rotation_str.toInt();

            break;
        }
    }

    return rotation;
}
