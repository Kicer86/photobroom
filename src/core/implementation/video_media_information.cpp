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
#include "exiftool_video_details_reader.hpp"
#include "video_media_information.hpp"


VideoMediaInformation::VideoMediaInformation(IConfiguration& configuration)
{
    const QVariant exiftoolVar = configuration.getEntry(ExternalToolsConfigKeys::exiftoolPath);

    m_exiftoolPath = exiftoolVar.toString();
}


FileInformation VideoMediaInformation::getInformation(const QString& path) const
{
    const auto output = ExiftoolUtils::exiftoolOutput(m_exiftoolPath, path);
    const auto parsed = ExiftoolUtils::parseOutput(output);
    const ExiftoolVideoDetailsReader videoDetailsReader(parsed);

    FileInformation info;
    info.common.dimension = videoDetailsReader.resolutionOf();
    info.common.creationTime = videoDetailsReader.creationTime();

    return info;
}
