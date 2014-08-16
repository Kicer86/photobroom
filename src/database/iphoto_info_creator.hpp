/*
 * Interface for PhotoInfoCreator
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef IPHOTO_INFO_CREATOR_HPP
#define IPHOTO_INFO_CREATOR_HPP

#include "iphoto_info.hpp"

class QString;

namespace Database
{
    struct IPhotoInfoCreator
    {
        virtual ~IPhotoInfoCreator() {}

        virtual IPhotoInfo::Ptr construct(const QString &) = 0;
    };
}

#endif
