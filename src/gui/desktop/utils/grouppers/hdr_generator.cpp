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

#include "hdr_generator.hpp"

#include "system/system.hpp"


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
    using GeneratorUtils::AISOutputAnalyzer;
    using GeneratorUtils::MagickOutputAnalyzer;

    // rotate photos
    const QStringList rotated = preparePhotos(m_data.photos, 100);

    // blend them!
    const int photos_count = static_cast<int>(m_data.photos.size());
    AISOutputAnalyzer analyzer(m_logger, photos_count);
    connect(&analyzer, &AISOutputAnalyzer::operation, this, &HDRGenerator::operation);
    connect(&analyzer, &AISOutputAnalyzer::progress,  this, &HDRGenerator::progress);
    connect(&analyzer, &AISOutputAnalyzer::finished,  this, &HDRGenerator::finished);

    emit operation(tr("generating HDR"));
    const QString location = System::getUniqueFileName(m_storage, "hdr");

    GeneratorUtils::execute(m_logger,
            m_data.alignImageStackPath,
            analyzer,
            m_runner,
            "-C",
            "-v",                              // for align_image_stack_output_analizer
            "-d", "-i", "-x", "-y", "-z",
            "-s", "0",
            "-o", location,
            rotated);

    const QString output = System::getUniqueFileName(m_storage, "jpeg");
    MagickOutputAnalyzer moa(m_logger, photos_count);
    connect(&moa, &MagickOutputAnalyzer::operation, this, &HDRGenerator::operation);
    connect(&moa, &MagickOutputAnalyzer::progress,  this, &HDRGenerator::progress);
    connect(&moa, &MagickOutputAnalyzer::finished,  this, &HDRGenerator::finished);

    emit operation(tr("Saving result"));

    GeneratorUtils::execute(m_logger,
            m_data.magickPath,
            moa,
            m_runner,
            "convert",
            "-monitor",                                      // for convert_output_analizer
            location,
            output);

    emit finished(output);
}
