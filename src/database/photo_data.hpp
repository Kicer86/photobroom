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

#include <any>

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
        Photo::Sha256sum     sha256Sum;
        Tag::TagsList        tags;
        Photo::FlagValues    flags;
        QString              path;
        QSize                geometry;
        GroupInfo            groupInfo;

        int getFlag(const Photo::FlagsE& flag) const;

        Data();
        Data(const Data &) = default;

        Data& operator=(const Data &) = default;
    };

    enum class Field
    {
        Checksum,
        Tags,
        Flags,
        Path,
        Geometry,
        GroupInfo,
    };

    template<Field>
    struct DeltaTypes {};

    template<>
    struct DeltaTypes<Field::Checksum>
    {
        typedef Photo::Sha256sum Storage;
    };

    template<>
    struct DeltaTypes<Field::Tags>
    {
        typedef Tag::TagsList Storage;
    };

    template<>
    struct DeltaTypes<Field::Flags>
    {
        typedef Photo::FlagValues Storage;
    };

    template<>
    struct DeltaTypes<Field::Path>
    {
        typedef QString Storage;
    };

    template<>
    struct DeltaTypes<Field::Geometry>
    {
        typedef QSize Storage;
    };

    template<>
    struct DeltaTypes<Field::GroupInfo>
    {
        typedef GroupInfo Storage;
    };

    class DATABASE_EXPORT DataDelta
    {
        public:
            DataDelta(): m_id(), m_data() {}

            DataDelta(const Photo::Id& id): m_id(id), m_data() {}

            template<Field field>
            void insert(const typename DeltaTypes<field>::Storage& value)
            {
                m_data.insert_or_assign(field, value);
            }

            void setId(const Photo::Id &);

            void clear();

            bool has(Field) const;
            const std::any& get(Field) const;

            template<Field field>
            const typename DeltaTypes<field>::Storage& get() const
            {
                typedef typename DeltaTypes<field>::Storage Result;

                const std::any& raw = get(field);

                // TODO: may fail then compiled with clang.
                // https://stackoverflow.com/questions/51693605/clang-compiled-program-throws-stdbad-any-cast-during-stdany-cast/51703166#51703166
                // https://bugs.llvm.org/show_bug.cgi?id=38485
                return std::any_cast<const Result &>(raw);
            }

            const Photo::Id& getId() const;

            bool operator<(const DataDelta &) const;

        private:
            Photo::Id                 m_id;
            std::map<Field, std::any> m_data;
    };

}

#endif // PHOTO_DATA_HPP
