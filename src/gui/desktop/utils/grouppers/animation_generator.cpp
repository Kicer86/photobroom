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

#include "animation_generator.hpp"

#include <cassert>

#include <QDirIterator>
#include <QFileInfo>
#include <QImage>
#include <webp_generator.hpp>

#include <core/function_wrappers.hpp>
#include <core/image_aligner.hpp>
#include <system/system.hpp>

using std::placeholders::_1;

///////////////////////////////////////////////////////////////////////////////


AnimationGenerator::AnimationGenerator(const Data& data, ILogger* logger, IExifReaderFactory& exif):
    GeneratorUtils::BreakableTask(data.storage, exif),
    m_data(data),
    m_logger(logger)
{

}


AnimationGenerator::~AnimationGenerator()
{

}


std::string AnimationGenerator::name() const
{
    return "AnimationGenerator";
}


void AnimationGenerator::run()
{
    emit progress(-1);

    // stabilize?
    try
    {
        const QStringList prepared = preparePhotos(m_data.photos, m_data.scale);
        const QStringList images_to_be_used = m_data.stabilize?
                                              stabilize(prepared):
                                              prepared;

        // generate animation (if there was no cancel during stabilization)
        const QString animation_path = generateAnimation(images_to_be_used);

        emit finished(animation_path);
    }
    catch(const QStringList& output)
    {
        emit error(tr("Error occured during external program execution"), output);
    }
}


QStringList AnimationGenerator::stabilize(const QStringList& photos)
{
    using GeneratorUtils::AISOutputAnalyzer;

    const int photos_count = static_cast<int>(photos.size());

    emit operation(tr("Preparing photos"));
    // https://groups.google.com/forum/#!topic/hugin-ptx/gqodoTgAjbI
    // http://wiki.panotools.org/Panorama_scripting_in_a_nutshell
    // http://wiki.panotools.org/Align_image_stack

    AISOutputAnalyzer analyzer(m_logger, photos_count);
    connect(&analyzer, &AISOutputAnalyzer::operation, this, &AnimationGenerator::operation);
    connect(&analyzer, &AISOutputAnalyzer::progress,  this, &AnimationGenerator::progress);
    connect(&analyzer, &AISOutputAnalyzer::finished,  this, &AnimationGenerator::finished);

    // generate aligned files
    emit operation(tr("Stabilizing photos"));
    const QString outputDir = m_tmpDir->path() + "/stabilized/";
    const auto alignedImages = ImageAligner(photos, *m_logger).registerProgress
    (
        [this](int photo, int photosCount)
        {
            emit progress(photo * 100 / photosCount);
        }
    ).align();

    if (alignedImages)
    {
        QDir().mkdir(outputDir);
        int i = 0;
        alignedImages->forEachImage([&](const auto& photo)
        {
            cv::imwrite(QString("%1/%2.tiff").arg(outputDir).arg(i++).toStdString(), photo);
        });
    }
    else
    {
        const QStringList& output = analyzer.tail();
        throw output;
    }

    QStringList stabilized_images;
    QDirIterator filesIterator(outputDir, {"*"}, QDir::Files);

    while(filesIterator.hasNext())
        stabilized_images.push_back(filesIterator.next());

    std::sort(stabilized_images.begin(), stabilized_images.end());

    return stabilized_images;
}


QString AnimationGenerator::generateAnimation(const QStringList& photos)
{
    // generate animation
    const QStringList all_but_last = photos.mid(0, photos.size() - 1);
    const QString last = photos.last();
    const QString location = System::getUniqueFileName(m_storage, "webp");

    emit operation(tr("Saving animated file"));

    WebPGenerator webpgenerator;
    webpgenerator
        .setDelay(std::chrono::milliseconds(static_cast<int>(1/m_data.fps * 1000)))
        .setLoopDelay(std::chrono::milliseconds(static_cast<int>(m_data.delay)))
        .setLossless();

    for (int i = 0; i < photos.size(); i++)
    {
        const auto& photoPath = photos[i];
        const QImage image(photoPath);
        webpgenerator.append(image);

        emit progress((i + 1) * 100 / static_cast<int>(photos.size()));
    }

    const auto outputData = webpgenerator.save();

    QFile outputFile(location);
    outputFile.open(QFile::WriteOnly);
    outputFile.write(outputData);

    return location;
}
