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
#include <memory>

#include <OpenLibrary/utils/data_ptr.hpp>

#include "core/photo_info.hpp"

struct IStreamFactory;

namespace Database
{
    class Entry
    {
        public:
            Entry(const std::shared_ptr<IStreamFactory>& stream = nullptr);
            Entry(const Entry&) = default;
            Entry(const APhotoInfo::Ptr &, const std::shared_ptr<IStreamFactory> &);
            Entry(Entry && );
            virtual ~Entry();

            virtual Entry& operator=(Entry && );
            virtual Entry& operator=(const Entry&) = default;

            typedef std::string hash;

            struct Data
            {
                Data(const std::shared_ptr<IStreamFactory>& stream = nullptr, const APhotoInfo::Ptr& photoInfo = nullptr);
                Data(const Data &) = default;

                Entry::hash m_hash;
                std::string m_path;         //path starts with 'file:' (when localfile), or with 'db:' (when in database)
                APhotoInfo::Ptr m_photoInfo;
                std::shared_ptr<IStreamFactory> m_stream;
            };

            data_ptr<Data> m_d;

        private:
            virtual bool operator==(const Entry&) const;
    };

}

#endif // ENTRY_HPP
