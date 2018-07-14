/*
 * HDR generator
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


HDRGenerator::HDRGenerator(const Data& data, ILogger* logger):
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

    auto rotateDir = System::getTmpDir("HDR_rotate");

    // rotate photos
    const QStringList rotated = rotatePhotos(m_data.photos, m_data.convertPath, m_logger, rotateDir->path());

    // blend them!
    const int photos_count = m_data.photos.size();
    AISOutputAnalyzer analyzer(m_logger, photos_count);
    connect(&analyzer, &AISOutputAnalyzer::operation, this, &HDRGenerator::operation);
    connect(&analyzer, &AISOutputAnalyzer::progress,  this, &HDRGenerator::progress);
    connect(&analyzer, &AISOutputAnalyzer::finished,  this, &HDRGenerator::finished);

    // generate aligned files
    emit operation(tr("generating HDR"));
    const QString output_prefix = m_tmpDir->path() + "/output";

    GeneratorUtils::execute(m_logger,
            m_data.alignImageStackPath,
            analyzer,
            m_runner,
            "-C",
            "-v",                              // for align_image_stack_output_analizer
            "-d", "-i", "-x", "-y", "-z",
            "-s", "0",
            "-o", output_prefix,
            rotated);

    emit finished(output_prefix + ".hdr");
}
