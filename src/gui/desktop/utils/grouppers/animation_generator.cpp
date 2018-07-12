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
#include <QEventLoop>
#include <QFileInfo>
#include <QRegExp>

#include <core/cross_thread_call.hpp>
#include <system/system.hpp>

#include "generator_utils.hpp"

using std::placeholders::_1;

namespace
{
    struct StabilizationData
    {
        const QRegExp cp_regExp   = QRegExp("^(Creating control points between|Optimizing Variables).*");
        const QRegExp run_regExp  = QRegExp("^Run called.*");
        const QRegExp save_regExp = QRegExp("^saving.*");

        int stabilization_steps = 0;
        int stabilization_step = 0;
        int photos_saved = 0;

        enum
        {
            StabilizingImages,
            SavingImages,
        } state = StabilizingImages;
    };
}


///////////////////////////////////////////////////////////////////////////////


AnimationGenerator::AnimationGenerator(const Data& data, ILogger* logger):
    m_data(data),
    m_cancelMutex(),
    m_tmpDir(System::getTmpDir("AG_tmp")),
    m_workingDir(System::persistentTmpDir("AG_wd")),
    m_logger(logger),
    m_cancel(false)
{
}


AnimationGenerator::~AnimationGenerator()
{

}


std::string AnimationGenerator::name() const
{
    return "GifGenerator";
}


void AnimationGenerator::perform()
{
    emit progress(-1);

    // stabilize?
    const QStringList images_to_be_used = m_data.stabilize?
                                          stabilize():
                                          m_data.photos;

    // generate gif (if there was no cancel during stabilization)
    const QString gif_path = m_cancel? "": generateGif(images_to_be_used);

    emit finished(gif_path);
}


void AnimationGenerator::cancel()
{
    std::lock_guard<std::mutex> lock(m_cancelMutex);
    m_cancel = true;

    emit canceled();
}


QStringList AnimationGenerator::stabilize()
{
    const int photos_count = m_data.photos.size();

    emit operation(tr("Stabilizing photos"));
    emit progress(0);
    // https://groups.google.com/forum/#!topic/hugin-ptx/gqodoTgAjbI
    // http://wiki.panotools.org/Panorama_scripting_in_a_nutshell
    // http://wiki.panotools.org/Align_image_stack

    // align_image_stack doesn't respect photo's rotation
    // generate rotated copies of original images

    int photo_index = 0;
    auto dirForRotatedPhotos = System::getTmpDir("AG_rotate");
    QStringList rotated_photos;
    StabilizationData stabilization_data;

    stabilization_data.stabilization_steps =  photos_count +   // 'photos_count' photos need orientation fixes
                                              photos_count - 1 // there will be n-1 control points groups
                                              + 4;             // and 4 optimization steps

    for (const QString& photo: m_data.photos)
    {
        const QString location = QString("%1/%2.tiff")
                                 .arg(dirForRotatedPhotos->path())
                                 .arg(photo_index);

        GeneratorUtils::execute(
            m_logger,
            m_data.convertPath,
            [](QIODevice &) {},
            std::bind(&AnimationGenerator::startAndWaitForFinish, this, _1),
            "-monitor",                                      // be verbose
            photo,
            "-auto-orient",
            location);

        rotated_photos << location;
        photo_index++;

        stabilization_data.stabilization_step++;

        emit progress( stabilization_data.stabilization_step * 100 /
                        stabilization_data.stabilization_steps);
    }

    // generate aligned files
    const QString output_prefix = m_tmpDir->path() + QDir::separator() + "stabilized";

    auto align_image_stack_output_analizer = [&stabilization_data, photos_count, this](QIODevice& device)
    {
        while(device.bytesAvailable() > 0 && device.canReadLine())
        {
            const QByteArray line_raw = device.readLine();
            const QString line(line_raw);

            const QString message = "align_image_stack: " + line.trimmed();
            m_logger->debug(message.toStdString());

            switch (stabilization_data.state)
            {
                case stabilization_data.StabilizingImages:
                    if (stabilization_data.cp_regExp.exactMatch(line))
                    {
                        stabilization_data.stabilization_step++;

                        emit progress( stabilization_data.stabilization_step * 100 /
                                       stabilization_data.stabilization_steps);
                    }
                    else if (stabilization_data.run_regExp.exactMatch(line))
                    {
                        stabilization_data.state = stabilization_data.SavingImages;

                        emit operation(tr("Saving stabilized images"));
                    }

                    break;

                case stabilization_data.SavingImages:
                    if (stabilization_data.save_regExp.exactMatch(line))
                    {
                        stabilization_data.photos_saved++;

                        emit progress( stabilization_data.photos_saved * 100 / photos_count );
                    }

                    break;
            }
        }
    };

    GeneratorUtils::execute(m_logger,
            m_data.alignImageStackPath,
            align_image_stack_output_analizer,
            std::bind(&AnimationGenerator::startAndWaitForFinish, this, _1),
            "-C",
            "-v",                              // for align_image_stack_output_analizer
            "--use-given-order",
            "-d", "-i", "-x", "-y", "-z",
            "-s", "0",
            "-a", output_prefix,
            rotated_photos);

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
    const QString location = System::getTmpFile(m_workingDir, "gif");

    ConvertOutputAnalyzer coa(m_logger, photos_count);
    connect(&coa, &ConvertOutputAnalyzer::operation, this, &AnimationGenerator::operation);
    connect(&coa, &ConvertOutputAnalyzer::progress,  this, &AnimationGenerator::progress);
    connect(&coa, &ConvertOutputAnalyzer::finished,  this, &AnimationGenerator::finished);

    emit operation(tr("Loading photos to be animated"));

    GeneratorUtils::execute(m_logger,
            m_data.convertPath,
            coa,
            std::bind(&AnimationGenerator::startAndWaitForFinish, this, _1),
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

    return m_cancel? "": location;

    // [1] It seems that align_image_stack may safe information about crop it applied to images.
    //     convert uses this information(?) and generates gif with frames moved
    //     from (0, 0) to (cropX, cropY). It results in a black border.
    //     +repage fixes it (I don't know how does it work exactly. It just does the trick).
    //     http://www.imagemagick.org/discourse-server/viewtopic.php?t=14556
}


void AnimationGenerator::startAndWaitForFinish(QProcess& process)
{
    // lock mutex to setup reaction on cancel correctly
    std::unique_lock<std::mutex> lock(m_cancelMutex);

    if (m_cancel == false)
    {
        QEventLoop loop;

        connect(this, &AnimationGenerator::canceled, &process, &QProcess::terminate);
        connect(&process, qOverload<int>(&QProcess::finished), &loop, &QEventLoop::exit);

        process.start();
        lock.unlock();
        loop.exec();
    }
}
