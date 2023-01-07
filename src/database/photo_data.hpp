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

#include <core/tag.hpp>
#include <core/ts_resource.hpp>

#include "database_export.h"
#include "group.hpp"
#include "photo_types.hpp"
#include "photo_data_fields.hpp"


namespace Photo
{
    class DataDelta;

    /**
     * @brief Structure containing full set of photo details
     */
    struct DATABASE_EXPORT Data
    {
        Photo::Id            id;
        Tag::TagsList        tags;
        Photo::FlagValues    flags;
        QString              path;
        QSize                geometry;
        GroupInfo            groupInfo;
        Photo::PHashT        phash;

        Data() = default;
        Data(const Data &) = default;
        Data(const Photo::Id& id);

        Data& operator=(const Data &) = default;
        bool operator==(const Data &) const = default;

        Data& apply(const DataDelta &);

        Q_PROPERTY(QString path MEMBER path)
        Q_PROPERTY(QSize size MEMBER geometry)
        Q_PROPERTY(Photo::Id id MEMBER id)
        Q_GADGET
    };


    /**
     * @brief Structure containing chosen of photo details
     */
    class DATABASE_EXPORT DataDelta
    {
            Q_PROPERTY(Photo::Id id MEMBER m_id CONSTANT)
            Q_GADGET

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
            typedef std::variant<
                DeltaTypes<Field::Tags>::Storage,
                DeltaTypes<Field::Flags>::Storage,
                DeltaTypes<Field::Path>::Storage,
                DeltaTypes<Field::Geometry>::Storage,
                DeltaTypes<Field::GroupInfo>::Storage,
                DeltaTypes<Field::PHash>::Storage
            > Storage;

            Photo::Id                m_id;
            std::unordered_map<Field, Storage> m_data;

            const Storage& get(Field) const;
            Storage& get(Field);
    };


    using SafeData = ol::ThreadSafeResource<Data>;
    using SharedData = std::shared_ptr<SafeData>;

    using SafeDataDelta = ol::ThreadSafeResource<DataDelta>;
    using SharedDataDelta = std::shared_ptr<SafeDataDelta>;
}

#endif // PHOTO_DATA_HPP
