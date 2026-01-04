/*
 * Photo Broom - photos management tool.
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

#include <QImage>
#include <opencv2/imgcodecs.hpp>

#include <core/image_aligner.hpp>
#include <core/hdr_assembler.hpp>

#include "hdr_generator.hpp"

import system;


HDRGenerator::HDRGenerator(const Data& data, ILogger* logger, IExifReaderFactory& exif):
    GeneratorUtils::BreakableTask(data.storage, exif),
    m_data(data),
    m_logger(logger)
{
}


std::string HDRGenerator::name() const
{
    return "HDRGenerator";
}


void HDRGenerator::run()
{
    // rotate photos
    const QStringList rotated = preparePhotos(m_data.photos, 100);

    // blend them!
    emit operation(tr("Stabilizing photos"));
    const auto alignedImages = ImageAligner(rotated, *m_logger).registerProgress
    (
        [this](int photo, int photosCount)
        {
            emit this->progress(photo * 100 / photosCount);
        }
    ).align();

    std::vector<cv::Mat> alignedMats;
    alignedImages->forEachImage([&alignedMats](const cv::Mat& mat) { alignedMats.push_back(mat); } );

    emit operation(tr("Geenrating HDR"));
    const auto hdrMat = HDR::assemble(alignedMats);

    emit operation(tr("Saving result"));
    const QString output = System::getUniqueFileName(m_storage, "jpeg");
    cv::imwrite(output.toStdString(), hdrMat);

    emit finished(output);
}
