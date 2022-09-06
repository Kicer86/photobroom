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

#ifndef HDRGENERATOR_HPP
#define HDRGENERATOR_HPP

#include "generator_utils.hpp"

class HDRGenerator: public GeneratorUtils::BreakableTask
{
        Q_OBJECT

    public:
        struct Data
        {
            QString storage;
            QString alignImageStackPath;
            QStringList photos;

            Data(): storage(), alignImageStackPath(), photos() {}
        };

        HDRGenerator(const Data& photos, ILogger *, IExifReaderFactory &);

        std::string name() const override;
        void run() override;

    private:
        const Data m_data;
        ILogger* m_logger;
};

#endif // HDRGENERATOR_HPP
