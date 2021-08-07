/*
 * Photo Broom - photos management tool.
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

#include <variant>
#include <QImage>
#include <OpenLibrary/putils/ts_resource.hpp>

#include <core/tag.hpp>

#include "database_export.h"
#include "group.hpp"
#include "photo_types.hpp"


namespace Photo
{
    class DataDelta;

    struct DATABASE_EXPORT Data
    {
        Photo::Id            id;
        Photo::Sha256sum     sha256Sum;
        Tag::TagsList        tags;
        Photo::FlagValues    flags;
        QString              path;
        QSize                geometry;
        GroupInfo            groupInfo;

        Data() = default;
        Data(const Data &) = default;

        Data& operator=(const Data &) = default;
        bool operator==(const Data &) const = default;

        Data& apply(const DataDelta &);

        Q_PROPERTY(QString path MEMBER path)
        Q_PROPERTY(QSize size MEMBER geometry)
        Q_PROPERTY(Photo::Id id MEMBER id)
        Q_GADGET
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

            explicit DataDelta(const Photo::Id& id): m_id(id), m_data() {}
            explicit DataDelta(const Data& oldData, const Data& newData);
            explicit DataDelta(const Data &);

            template<Field field>
            void insert(const typename DeltaTypes<field>::Storage& value)
            {
                m_data.insert_or_assign(field, value);
            }

            void setId(const Photo::Id &);
            void clear();
            bool has(Field) const;

            template<Field field>
            const typename DeltaTypes<field>::Storage& get() const
            {
                typedef typename DeltaTypes<field>::Storage Result;

                const Storage& raw = get(field);

                return std::get<Result>(raw);
            }

            template<Field field>
            typename DeltaTypes<field>::Storage& get()
            {
                typedef typename DeltaTypes<field>::Storage Result;

                Storage& raw = get(field);

                return std::get<Result>(raw);
            }

            const Photo::Id& getId() const;

            bool operator<(const DataDelta &) const;
            bool operator==(const DataDelta &) const;
            DataDelta& operator|=(const DataDelta &);       // merge anothor delta into
            DataDelta& operator=(const Data &);

        private:
            typedef std::variant<DeltaTypes<Field::Checksum>::Storage,
                                 DeltaTypes<Field::Tags>::Storage,
                                 DeltaTypes<Field::Flags>::Storage,
                                 DeltaTypes<Field::Path>::Storage,
                                 DeltaTypes<Field::Geometry>::Storage,
                                 DeltaTypes<Field::GroupInfo>::Storage> Storage;

            Photo::Id                m_id;
            std::unordered_map<Field, Storage> m_data;

            const Storage& get(Field) const;
            Storage& get(Field);
    };

    using SafeData = ol::ThreadSafeResource<Data>;
    using SharedData = std::shared_ptr<SafeData>;
}

#endif // PHOTO_DATA_HPP
