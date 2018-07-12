/*
 * generic code for generators
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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

#include "generator_utils.hpp"


namespace
{
    const QRegExp loadImages_regExp = QRegExp(R"(^Load\/Image\/.*100% complete.*)");
    const QRegExp mogrify_regExp    = QRegExp(R"(^Mogrify\/Image\/.*)");
    const QRegExp dither_regExp     = QRegExp(R"(^Dither\/Image\/.*100% complete.*)");
}

namespace GeneratorUtils
{

    ConvertOutputAnalyzer::ConvertOutputAnalyzer(ILogger* logger, int photos_count):
        m_photos_count(photos_count),
        m_logger(logger)
    {
    }


    void ConvertOutputAnalyzer::operator()(QIODevice& device)
    {
        while(device.bytesAvailable() > 0 && device.canReadLine())
        {
            const QByteArray line_raw = device.readLine();
            const QString line(line_raw);

            const QString message = "convert: " + line.trimmed();
            m_logger->debug(message.toStdString());

            switch (conversion_data.state)
            {
                case conversion_data.LoadingImages:
                {
                    if (loadImages_regExp.exactMatch(line))
                    {
                        conversion_data.photos_loaded++;

                        emit progress(conversion_data.photos_loaded * 100 / m_photos_count);
                    }
                    else if (mogrify_regExp.exactMatch(line))
                    {
                        conversion_data.state = conversion_data.BuildingImage;

                        emit operation(tr("Assembling final file"));
                    }

                    break;
                }

                case conversion_data.BuildingImage:
                {
                    if (dither_regExp.exactMatch(line))
                    {
                        conversion_data.photos_assembled++;

                        emit progress(conversion_data.photos_assembled * 100 / m_photos_count);
                    }

                    break;
                }
            };
        }
    }

}
