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

#include <cassert>

#include <QVariant>

#include "constants.hpp"
#include "iconfiguration.hpp"
#include "ffmpeg_video_details_reader.hpp"


FFmpegMediaInformation::FFmpegMediaInformation(): m_ffmpegPath()
{
}


void FFmpegMediaInformation::set(IConfiguration* configuration)
{
    const QVariant ffmpegVar = configuration->getEntry(ExternalToolsConfigKeys::ffmpegPath);

    m_ffmpegPath = ffmpegVar.toString();
}


bool FFmpegMediaInformation::canHandle(const QString& path) const
{
    return true;
}


QSize FFmpegMediaInformation::size(const QString& path) const
{
    assert(m_ffmpegPath.isEmpty() == false);

    const FFMpegVideoDetailsReader videoDetailsReader(m_ffmpegPath);
    const QSize resolution = videoDetailsReader.resolutionOf(path);

    return resolution;
}
