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


namespace Database
{
    typedef std::string Sha256sum;

    struct DATABASE_EXPORT Id
    {
            typedef int type;

            Id();
            explicit Id(type);
            Id(const Id &) = default;

            Id& operator=(const Id &) = default;
            operator type() const;
            bool operator!() const;
            bool valid() const;
            type value() const;

        private:
            type m_value;
            bool m_valid;
    };

    enum class FlagsE
    {
        StagingArea,
        ExifLoaded,
        Sha256Loaded,
        ThumbnailLoaded,
    };

    typedef std::map<FlagsE, int> FlagValues;

    struct DATABASE_EXPORT PhotoData
    {
        Id            id;
        Sha256sum     sha256Sum;
        Tag::TagsList tags;
        FlagValues    flags;
        QString       path;
        QImage        thumbnail;

        int getFlag(const FlagsE& flag) const;
    };


    struct PhotoInfoIdHash
    {
        std::size_t operator()(const Database::Id& key) const
        {
            return std::hash<Database::Id::type>()(key.value());
        }
    };

}

#endif // PHOTO_DATA_HPP
