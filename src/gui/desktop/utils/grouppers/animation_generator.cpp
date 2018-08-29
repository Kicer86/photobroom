/*
 * tool for generating gif file from many images
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

#include <QDirIterator>
#include <QFileInfo>

#include <core/cross_thread_call.hpp>
#include <system/system.hpp>

using std::placeholders::_1;

///////////////////////////////////////////////////////////////////////////////


AnimationGenerator::AnimationGenerator(const Data& data, ILogger* logger):
    GeneratorUtils::BreakableTask(data.storage),
    m_data(data),
    m_logger(logger)
{

}


AnimationGenerator::~AnimationGenerator()
{

}


std::string AnimationGenerator::name() const
{
    return "GifGenerator";
}


void AnimationGenerator::run()
{
    emit progress(-1);

    // stabilize?
    try
    {
        const QStringList images_to_be_used = m_data.stabilize?
                                            stabilize():
                                            m_data.photos;

        // generate gif (if there was no cancel during stabilization)
        const QString gif_path = generateGif(images_to_be_used);

        emit finished(gif_path);
    }
    catch(const QStringList& output)
    {
        emit error(tr("Error occured during external program execution"),
                   output);
    }
}


QStringList AnimationGenerator::stabilize()
{
    using GeneratorUtils::AISOutputAnalyzer;

    const int photos_count = m_data.photos.size();

    emit operation(tr("Preparing photos"));
    emit progress(0);
    // https://groups.google.com/forum/#!topic/hugin-ptx/gqodoTgAjbI
    // http://wiki.panotools.org/Panorama_scripting_in_a_nutshell
    // http://wiki.panotools.org/Align_image_stack

    // align_image_stack doesn't respect photo's rotation
    // generate rotated copies of original images
    auto dirForRotatedPhotos = System::getTmpDir("AG_rotate");
    const QStringList rotated_photos = rotatePhotos(m_data.photos, m_data.convertPath, m_logger, dirForRotatedPhotos->path());

    AISOutputAnalyzer analyzer(m_logger, photos_count);
    connect(&analyzer, &AISOutputAnalyzer::operation, this, &AnimationGenerator::operation);
    connect(&analyzer, &AISOutputAnalyzer::progress,  this, &AnimationGenerator::progress);
    connect(&analyzer, &AISOutputAnalyzer::finished,  this, &AnimationGenerator::finished);

    // generate aligned files
    emit operation(tr("Stabilizing photos"));
    const QString output_prefix = m_tmpDir->path() + "/stabilized";

    GeneratorUtils::execute(m_logger,
            m_data.alignImageStackPath,
            analyzer,
            m_runner,
            "-C",
            "-v",                              // for align_image_stack_output_analizer
            "--use-given-order",
            "-d", "-i", "-x", "-y", "-z",
            "-s", "0",
            "-a", output_prefix,
            rotated_photos);

    if (m_runner.getExitCode() != 0)
    {
        const QStringList& output = analyzer.tail();
        throw output;
    }

    QStringList stabilized_images;

    const QFileInfo output_prefix_info(output_prefix);
    QDirIterator filesIterator(output_prefix_info.absolutePath(), {output_prefix_info.fileName() + "*"}, QDir::Files);

    while(filesIterator.hasNext())
        stabilized_images.push_back(filesIterator.next());

    std::sort(stabilized_images.begin(), stabilized_images.end());

    return stabilized_images;
}


QString AnimationGenerator::generateGif(const QStringList& photos)
{
    using GeneratorUtils::ConvertOutputAnalyzer;

    // generate gif
    const int photos_count = m_data.photos.size();
    const double last_photo_exact_delay = (m_data.delay / 1000.0) * 100 + (1 / m_data.fps * 100);
    const int last_photo_delay = static_cast<int>(last_photo_exact_delay);
    const QStringList all_but_last = photos.mid(0, photos.size() - 1);
    const QString last = photos.last();
    const QString location = System::getTmpFile(m_storage, "gif");

    ConvertOutputAnalyzer coa(m_logger, photos_count);
    connect(&coa, &ConvertOutputAnalyzer::operation, this, &AnimationGenerator::operation);
    connect(&coa, &ConvertOutputAnalyzer::progress,  this, &AnimationGenerator::progress);
    connect(&coa, &ConvertOutputAnalyzer::finished,  this, &AnimationGenerator::finished);

    emit operation(tr("Loading photos to be animated"));

    GeneratorUtils::execute(m_logger,
            m_data.convertPath,
            coa,
            m_runner,
            "-monitor",                                      // for convert_output_analizer
            "-delay", QString::number(1/m_data.fps * 100),   // convert fps to 1/100th of a second
            all_but_last,
            "-delay", QString::number(last_photo_delay),
            last,
            "+repage",                                       // [1]
            "-auto-orient",
            "-loop", "0",
            "-scale", QString::number(m_data.scale) + "%",
            location);

    return location;

    // [1] It seems that align_image_stack may safe information about crop it applied to images.
    //     convert uses this information(?) and generates gif with frames moved
    //     from (0, 0) to (cropX, cropY). It results in a black border.
    //     +repage fixes it (I don't know how does it work exactly. It just does the trick).
    //     http://www.imagemagick.org/discourse-server/viewtopic.php?t=14556
}
