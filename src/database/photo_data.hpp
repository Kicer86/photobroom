/*
 * Basic data about photo
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

#ifndef PHOTO_DATA_HPP
#define PHOTO_DATA_HPP

#include <QImage>

#include <core/tag.hpp>

#include "database_export.h"
#include "group.hpp"
#include "photo_types.hpp"


namespace Photo
{

    struct DATABASE_EXPORT Data
    {
        Photo::Id            id;
        Group::Id            group_id;
        Photo::Sha256sum     sha256Sum;
        Tag::TagsList        tags;
        Photo::FlagValues    flags;
        QString              path;
        QSize                geometry;

        int getFlag(const Photo::FlagsE& flag) const;

        Data();
        Data(const Data &) = default;

        Data& operator = (const Data &) = default;
    };

}

#endif // PHOTO_DATA_HPP
