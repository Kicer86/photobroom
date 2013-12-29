/*
    Database entries manipulator
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef DATABASE_ENTRY_HPP
#define DATABASE_ENTRY_HPP

#include <string>

#include <boost/cstdint.hpp>

#include <OpenLibrary/utils/data_ptr.hpp>

#include "core/photo_info.hpp"

namespace Database
{
    class Entry
    {
        public:
            Entry();
            Entry(const Entry&) = default;
            Entry(const APhotoInfo::Ptr &);
            Entry(Entry && );
            virtual ~Entry();

            virtual Entry& operator=(Entry && );
            virtual Entry& operator=(const Entry&) = default;

            typedef boost::uint32_t crc32;

            struct Data
            {
                Data(const APhotoInfo::Ptr& photoInfo = nullptr): m_crc(0xffffffff), m_path("null"), m_photoInfo(photoInfo) {}

                crc32       m_crc;
                std::string m_path;         //path starts with 'file:' (when localfile), or with 'db:' (when in database)
                APhotoInfo::Ptr  m_photoInfo;
            };

            data_ptr<Data> m_d;

        private:
            virtual bool operator==(const Entry&) const;
    };

}

#endif // ENTRY_HPP
