/*
 * SQLDatabase iterator
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

#ifndef PHOTOITERATOR_H
#define PHOTOITERATOR_H

#include <QVariant>

#include <utils/data_ptr.hpp>

#include <core/aphoto_info.hpp>

#include "database_export.h"

class QSqlQuery;
class APhotoInfo;

namespace Database
{
    struct IQuery
    {
        enum class Fields
        {
            Id,
            Path,
            Hash,
            TagName,
            TagValue,
            TagType,
        };

        virtual ~IQuery() {}

        virtual bool gotoNext() = 0;                             //move to next data entry
        virtual QVariant getField(Fields) = 0;          //get value for given name in current entry
        virtual bool valid() const = 0;

        virtual std::shared_ptr<IQuery> clone() = 0;
    };

    class DATABASE_EXPORT PhotoIterator
    {
        public:
            PhotoIterator(const std::shared_ptr<IQuery> &);
            PhotoIterator(const PhotoIterator &) = default;
            PhotoIterator();
            virtual ~PhotoIterator();

            operator bool();
            bool operator !();

            PhotoIterator& operator++();
            PhotoIterator operator++(int);
            PhotoIterator& operator=(const PhotoIterator &) = default;
            IPhotoInfo* operator->();

        private:
            struct Impl;
            data_ptr<Impl> m_impl;
    };
}

#endif // PHOTOITERATOR_H
