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

#ifndef ANIMATION_GENERATOR_HPP
#define ANIMATION_GENERATOR_HPP

#include <QSize>

#include "generator_utils.hpp"

class QProcess;

struct ILogger;

class AnimationGenerator: public GeneratorUtils::BreakableTask
{
        Q_OBJECT

    public:
        struct Data
        {
            QString storage;
            QString alignImageStackPath;
            QStringList photos;
            double fps;
            double delay;
            int scale;                          // % of original size
            bool stabilize;

            Data(): storage(), alignImageStackPath(), photos(), fps(0.0), delay(0.0), scale(100), stabilize(false) {}
        };

        AnimationGenerator(const Data& data, ILogger *, IExifReaderFactory &);
        AnimationGenerator(const AnimationGenerator &) = delete;
        ~AnimationGenerator();

        AnimationGenerator& operator=(const AnimationGenerator &) = delete;

        std::string name() const override;
        void run() override;

    private:
        Data m_data;
        ILogger* m_logger;

        QStringList stabilize(const QStringList &);
        QString generateAnimation(const QStringList &);
};


#endif // ANIMATION_GENERATOR_HPP
