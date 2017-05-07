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

#ifndef ANIMATION_GENERATOR_HPP
#define ANIMATION_GENERATOR_HPP

#include <QObject>
#include <QSize>

#include <core/itask_executor.hpp>

class QMovie;


struct AnimationGenerator: QObject
{
    struct Data
    {
        double fps;
        double delay;
        double scale;
        QStringList photos;

        Data(): fps(0.0), delay(0.0), scale(0.0), photos() {}
    };

    struct GifGenerator: ITaskExecutor::ITask
    {
        GifGenerator(const AnimationGenerator::Data& data, const QString& location, const std::function<void(const QString &)>& doneCallback);

        std::string name() const override;
        void perform() override;

        AnimationGenerator::Data m_data;
        QString m_location;
        std::function<void(const QString &)> m_doneCallback;
    };

    AnimationGenerator(ITaskExecutor* executor, const std::function<void(QWidget *, const QString &)>& callback);
    AnimationGenerator(const AnimationGenerator &) = delete;
    ~AnimationGenerator();

    AnimationGenerator& operator=(const AnimationGenerator &) = delete;


    void generatePreviewWidget(const Data& data);
    void scalePreview(double scale);
    void done(const QString& location);

    std::function<void(QWidget *, const QString &)> m_callback;
    std::unique_ptr<QMovie> m_movie;
    QSize m_baseSize;
    ITaskExecutor* m_executor;
};


#endif // ANIMATION_GENERATOR_HPP
