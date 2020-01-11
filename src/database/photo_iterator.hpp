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

#ifndef PHOTOITERATOR_H
#define PHOTOITERATOR_H

#include <OpenLibrary/utils/data_ptr.hpp>

#include <database/iphoto_info.hpp>

#include "database_export.h"

class QSqlQuery;

namespace Database
{
    struct IBackend;

    //IQuery represents a result of database query.
    //It can access one row of data at once.
    struct DATABASE_DEPRECATED IQuery
    {
        virtual ~IQuery() {}

        virtual bool gotoNext() = 0;                    //move to next data row
        virtual IPhotoInfo::Id getId() const = 0;        //get photo Id
        virtual bool valid() const = 0;
        virtual int size() const = 0;                   //number of rows
        virtual IBackend* backend() const = 0;          //source backend

        virtual IQuery* clone() const = 0;
    };

    template<typename T>
    class DATABASE_DEPRECATED InterfaceContainer final
    {
        public:
            InterfaceContainer(): m_data()
            {
            }

            InterfaceContainer(T* i): m_data(i)         //container takes care of interface
            {

            }

            InterfaceContainer(const InterfaceContainer& other): m_data(other->clone())
            {

            }

            ~InterfaceContainer()
            {

            }

            IQuery* operator*() const
            {
                return m_data.get();
            }

            IQuery* operator->() const
            {
                return m_data.get();
            }

            InterfaceContainer& operator=(const InterfaceContainer& other)
            {
                m_data.reset(other->clone());

                return *this;
            }

        private:
            std::unique_ptr<T> m_data;
    };

    class DATABASE_DEPRECATED_EXPORT PhotoIterator
    {
        public:
            PhotoIterator(const InterfaceContainer<IQuery> &);
            PhotoIterator(const PhotoIterator &) = default;
            PhotoIterator();
            virtual ~PhotoIterator();

            operator bool() const;
            bool operator !() const;

            PhotoIterator&  operator++();
            PhotoIterator   operator++(int);
            PhotoIterator&  operator=(const PhotoIterator &) = default;
            IPhotoInfo::Ptr operator->();
            IPhotoInfo::Ptr operator*();

            const IQuery* query() const;

        private:
            struct Impl;
            data_ptr<Impl> m_impl;
    };
}

#endif // PHOTOITERATOR_H
