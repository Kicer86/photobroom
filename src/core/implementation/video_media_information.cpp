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


#include <cassert>

#include <QVariant>

#include "constants.hpp"
#include "iconfiguration.hpp"
#include "ffmpeg_video_details_reader.hpp"
#include "video_media_information.hpp"


VideoMediaInformation::VideoMediaInformation(IConfiguration& configuration):
    m_ffprobePath()
{
    const QVariant ffprobeVar = configuration.getEntry(ExternalToolsConfigKeys::ffprobePath);

    m_ffprobePath = ffprobeVar.toString();
}


std::optional<QSize> VideoMediaInformation::size(const QString& path) const
{
    assert(m_ffprobePath.isEmpty() == false);

    const FFMpegVideoDetailsReader videoDetailsReader(m_ffprobePath);
    const std::optional<QSize> resolution = videoDetailsReader.resolutionOf(path);

    return resolution;
}


FileInformation VideoMediaInformation::getInformation(const QString& path) const
{
    FileInformation info;
    info.common.dimension = *size(path);

    return info;
}
