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

#include <QMovie>
#include <QLabel>
#include <QProcess>
#include <QProgressBar>

#include <core/cross_thread_call.hpp>
#include <system/system.hpp>


AnimationGenerator::GifGenerator::GifGenerator(const AnimationGenerator::Data& data, const QString& location, const std::function<void(const QString &)>& doneCallback):
    m_data(data),
    m_location(location),
    m_doneCallback(doneCallback)
{
}


std::string AnimationGenerator::GifGenerator::name() const
{
    return "GifGenerator";
}


void AnimationGenerator::GifGenerator::perform()
{
    const int last_photo_delay = (m_data.delay / 1000.0) * 100 + (1 / m_data.fps * 100);
    const QStringList all_but_last = m_data.photos.mid(0, m_data.photos.size() - 1);
    const QString last = m_data.photos.last();

    QStringList args;
    args << "-delay" << QString::number(1/m_data.fps * 100);   // convert fps to 1/100th of a second
    args << all_but_last;
    args << "-delay" << QString::number(last_photo_delay);
    args << last;
    args << "-auto-orient";
    args << "-loop" << "0";
    args << "-resize" << QString::number(m_data.scale) + "%";
    args << m_location;

    QProcess convert;
    convert.start("convert", args);
    convert.waitForFinished(-1);

    m_doneCallback(m_location);
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
