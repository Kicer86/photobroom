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

#ifndef PHOTOINFORMATION_HPP
#define PHOTOINFORMATION_HPP

#include "imedia_information.hpp"

#include "core_export.h"

struct IExifReaderFactory;


class CORE_EXPORT Eviv2MediaInformation
{
    public:
        explicit Eviv2MediaInformation(IExifReaderFactory &);
        Eviv2MediaInformation(const Eviv2MediaInformation &) = delete;

        Eviv2MediaInformation& operator=(const Eviv2MediaInformation &) = delete;

        std::optional<QSize> size(const QString &) const;

    private:
        IExifReaderFactory& m_exif;
};

#endif // PHOTOINFORMATION_H
