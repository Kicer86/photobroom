/*
 * Toolkit for reading video information
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

#include "video_information.hpp"

#include <QProcess>
#include <QString>


VideoInformation::VideoInformation()
{
}


QSize VideoInformation::size(const QString& path) const
{
    QProcess ffmpeg_process;
    ffmpeg_process.setProcessChannelMode(QProcess::MergedChannels);

    const QStringList ffmpeg_args = { "-i", path };

    ffmpeg_process.start("ffmpeg", ffmpeg_args );
    bool status = ffmpeg_process.waitForFinished();

    QSize result;

    if (status)
    {
        const QByteArray output = ffmpeg_process.readAll();
        const QString output_str = output.constData();

        QRegExp resolution_regex(".*Stream [^ ]+ Video:.*, ([0-9]+)x([0-9]+).*");

        const bool matched = resolution_regex.exactMatch(output_str);

        if (matched)
        {
            const QStringList captured = resolution_regex.capturedTexts();
            const QString resolution_x_str = captured[1];
            const QString resolution_y_str = captured[2];

            const int resolution_x = resolution_x_str.toInt();
            const int resolution_y = resolution_y_str.toInt();

            result = QSize(resolution_x, resolution_y);
        }
    }

    return result;
}
