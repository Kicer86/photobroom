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

#include <cmath>

#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QProcess>
#include <QRegExp>
#include <QTextStream>

#include <core/cross_thread_call.hpp>
#include <system/system.hpp>

namespace
{
    template<typename T>
    void append(QStringList& list, const T& arg)
    {
        list << arg;
    }

    template<typename T, typename ...Args>
    void append(QStringList& list, const T& arg, Args... args)
    {
        list << arg;
        append(list, args...);
    }

    template<typename ...Args>
    bool execute(const QString& executable, const std::function<void(QIODevice &)>& outputDataCallback, Args... args)
    {
        QStringList arguments;
        append(arguments, args...);

        QProcess pr;
        pr.setProcessChannelMode(QProcess::MergedChannels);

        if (outputDataCallback)
            QObject::connect(&pr, &QIODevice::readyRead, std::bind(outputDataCallback, std::ref(pr)));

        pr.start(executable, arguments);
        const bool status = pr.waitForFinished(-1);

        return status;
    }
}


///////////////////////////////////////////////////////////////////////////////


AnimationGenerator::AnimationGenerator(const Data& data):
    m_data(data)
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
    const QStringList images_to_be_used = m_data.stabilize? stabilize(): m_data.photos;
    generateGif(images_to_be_used);
}


QStringList AnimationGenerator::stabilize()
{
    const int photos_count = m_data.photos.size();

    emit operation(tr("Stabilizing photos"));
    emit progress(0);
    // https://groups.google.com/forum/#!topic/hugin-ptx/gqodoTgAjbI
    // http://wiki.panotools.org/Panorama_scripting_in_a_nutshell
    // http://wiki.panotools.org/Align_image_stack

    // generate aligned files
    const QString output_prefix = System::getTempFilePath() + "_";

    struct
    {
        const QRegExp cp_regExp   = QRegExp("^Creating control points between.*");
        const QRegExp opt_regExp  = QRegExp("^Optimizing Variables.*");
        const QRegExp save_regExp = QRegExp("^saving.*");

        int photos_stabilized = 0;
        int photos_saved = 0;

        enum
        {
            StabilizingImages,
            SavingImages,
        } state = StabilizingImages;

    } stabilization_data;

    auto align_image_stack_output_analizer = [&stabilization_data, photos_count, this](QIODevice& device)
    {
        while(device.bytesAvailable() > 0 && device.canReadLine())
        {
            const QByteArray line_raw = device.readLine();
            const QString line(line_raw);

            switch (stabilization_data.state)
            {
                case stabilization_data.StabilizingImages:
                    if (stabilization_data.cp_regExp.exactMatch(line))
                    {
                        stabilization_data.photos_stabilized++;

                        emit progress( stabilization_data.photos_stabilized * 100 / (photos_count - 1));   // there will be n-1 control points groups
                    }
                    else if (stabilization_data.opt_regExp.exactMatch(line))
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

    execute("align_image_stack",
            align_image_stack_output_analizer,
            "-C",
            "-v",                              // for align_image_stack_output_analizer
            "--use-given-order",
            "-d", "-i", "-x", "-y", "-z",
            "-s", "0",
            "-a", output_prefix,
            m_data.photos);

    QStringList stabilized_images;

    const QFileInfo output_prefix_info(output_prefix);
    QDirIterator filesIterator(output_prefix_info.absolutePath(), {output_prefix_info.fileName() + "*"}, QDir::Files);

    while(filesIterator.hasNext())
        stabilized_images.push_back(filesIterator.next());

    std::sort(stabilized_images.begin(), stabilized_images.end());

    return stabilized_images;
}


void AnimationGenerator::generateGif(const QStringList& photos)
{
    // generate gif
    const int photos_count = m_data.photos.size();
    const int last_photo_delay = (m_data.delay / 1000.0) * 100 + (1 / m_data.fps * 100);
    const QStringList all_but_last = photos.mid(0, photos.size() - 1);
    const QString last = photos.last();
    const QString location = System::getTempFilePath() + ".gif";

    struct
    {
        const QRegExp loadImages_regExp = QRegExp(R"(^Load\/Image\/.*100% complete.*)");
        const QRegExp mogrify_regExp    = QRegExp(R"(^Mogrify\/Image\/.*)");
        const QRegExp dither_regExp     = QRegExp(R"(^Dither\/Image\/.*100% complete.*)");
        int photos_loaded = 0;
        int photos_assembled = 0;

        enum
        {
            LoadingImages,
            BuildingGif,
        } state = LoadingImages;

    } conversion_data;

    emit operation(tr("Loading photos to be animated"));

    auto convert_output_analizer = [&conversion_data, &photos_count, this](QIODevice& device)
    {
        while(device.bytesAvailable() > 0 && device.canReadLine())
        {
            const QByteArray line_raw = device.readLine();
            const QString line(line_raw);

            switch (conversion_data.state)
            {
                case conversion_data.LoadingImages:
                {
                    if (conversion_data.loadImages_regExp.exactMatch(line))
                    {
                        conversion_data.photos_loaded++;

                        emit progress(conversion_data.photos_loaded * 100 / photos_count);
                    }
                    else if (conversion_data.mogrify_regExp.exactMatch(line))
                    {
                        conversion_data.state = conversion_data.BuildingGif;

                        emit operation(tr("Assembling gif file"));
                    }

                    break;
                }

                case conversion_data.BuildingGif:
                {
                    if (conversion_data.dither_regExp.exactMatch(line))
                    {
                        conversion_data.photos_assembled++;

                        emit progress(conversion_data.photos_assembled * 100 / photos_count);
                    }

                    break;
                }
            };
        }
    };

    execute("convert",
            convert_output_analizer,
            "-monitor",                                      // for convert_output_analizer
            "-delay", QString::number(1/m_data.fps * 100),   // convert fps to 1/100th of a second
            all_but_last,
            "-delay", QString::number(last_photo_delay),
            last,
            "-auto-orient",
            "-loop", "0",
            "-resize", QString::number(m_data.scale) + "%",
            location);

    emit finished(location);
}
