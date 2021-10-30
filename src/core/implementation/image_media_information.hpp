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


class CORE_EXPORT ImageMediaInformation
{
    public:
        explicit ImageMediaInformation(IExifReaderFactory &);
        ImageMediaInformation(const ImageMediaInformation &) = delete;

        ImageMediaInformation& operator=(const ImageMediaInformation &) = delete;

        std::optional<QSize> size(const QString &) const;

    private:
        IExifReaderFactory& m_exif;
};

#endif // PHOTOINFORMATION_H
