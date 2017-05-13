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
#include <QMovie>
#include <QLabel>
#include <QProcess>
#include <QProgressBar>
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

        if (outputDataCallback)
            QObject::connect(&pr, &QIODevice::readyRead, std::bind(outputDataCallback, std::ref(pr)));

        pr.start(executable, arguments);
        const bool status = pr.waitForFinished(-1);

        return status;
    }

    struct GifGenerator: ITaskExecutor::ITask
    {
        GifGenerator(const AnimationGenerator::Data& data, const QString& location, const std::function<void(const QString &)>& doneCallback):
            m_data(data),
            m_location(location),
            m_doneCallback(doneCallback)
        {
        }

        std::string name() const override
        {
            return "GifGenerator";
        }

        void perform() override
        {
            // stabilize?
            QStringList images_to_be_used;

            if (m_data.stabilize)
            {
                // https://groups.google.com/forum/#!topic/hugin-ptx/gqodoTgAjbI
                // http://wiki.panotools.org/Panorama_scripting_in_a_nutshell
                // http://wiki.panotools.org/Align_image_stack

                // generate aligned files
                const QString output_prefix = System::getTempFilePath() + "_";

                int photos_left = m_data.photos.size();

                auto align_image_stack_output_analizer = [&photos_left](QIODevice& device)
                {
                    QRegExp cp_regExp("Creating control points between.*");

                    while(device.bytesAvailable() > 0)
                    {
                        const QByteArray line_raw = device.readLine();
                        const QString line(line_raw);

                        if (cp_regExp.exactMatch(line))
                        {
                            photos_left--;

                        }
                    }
                };

                execute("align_image_stack",
                        align_image_stack_output_analizer,
                        "-C",
                        "--use-given-order",
                        "-d", "-i", "-x", "-y", "-z",
                        "-s", "0",
                        "-a", output_prefix,
                        m_data.photos);

                const QFileInfo output_prefix_info(output_prefix);
                QDirIterator filesIterator(output_prefix_info.absolutePath(), {output_prefix_info.fileName() + "*"}, QDir::Files);

                while(filesIterator.hasNext())
                    images_to_be_used.push_back(filesIterator.next());

                std::sort(images_to_be_used.begin(), images_to_be_used.end());
            }
            else
                images_to_be_used = m_data.photos;

            // generate gif
            const int last_photo_delay = (m_data.delay / 1000.0) * 100 + (1 / m_data.fps * 100);
            const QStringList all_but_last = images_to_be_used.mid(0, images_to_be_used.size() - 1);
            const QString last = images_to_be_used.last();

            execute("convert",
                    nullptr,
                    "-delay", QString::number(1/m_data.fps * 100),   // convert fps to 1/100th of a second
                    all_but_last,
                    "-delay", QString::number(last_photo_delay),
                    last,
                    "-auto-orient",
                    "-loop", "0",
                    "-resize", QString::number(m_data.scale) + "%",
                    m_location);

            m_doneCallback(m_location);
        }

        AnimationGenerator::Data m_data;
        QString m_location;
        std::function<void(const QString &)> m_doneCallback;
    };
}


///////////////////////////////////////////////////////////////////////////////


AnimationGenerator::AnimationGenerator(ITaskExecutor* executor, const std::function<void(QWidget *, const QString &)>& callback):
    m_callback(callback),
    m_movie(),
    m_baseSize(),
    m_executor(executor)
{
}


AnimationGenerator::~AnimationGenerator()
{

}


void AnimationGenerator::generatePreviewWidget(const Data& data)
{
    if (m_movie.get() != nullptr)
        m_movie->stop();

    m_baseSize = QSize();

    const QString location = System::getTempFilePath() + ".gif";

    using namespace std::placeholders;
    std::function<void(const QString &)> doneFun = std::bind(&AnimationGenerator::done, this, _1);
    auto doneCallback = make_cross_thread_function(this, doneFun);

    auto task = std::make_unique<GifGenerator>(data, location, doneCallback);
    m_executor->add(std::move(task));

    QProgressBar* progress = new QProgressBar;
    progress->setRange(0, 0);

    m_callback(progress, QString());
}


void AnimationGenerator::scalePreview(double scale)
{
    if (m_movie.get() != nullptr)
    {
        if (m_baseSize.isValid() == false)
            m_baseSize = m_movie->frameRect().size();

        const double scaleFactor = scale/100;
        QSize size = m_baseSize;
        size.rheight() *= scaleFactor;
        size.rwidth() *= scaleFactor;

        m_movie->setScaledSize(size);
    }
}


void AnimationGenerator::done(const QString& location)
{
    m_movie = std::make_unique<QMovie>(location);
    QLabel* label = new QLabel;

    label->setMovie(m_movie.get());
    m_movie->start();

    m_callback(label, location);
}
