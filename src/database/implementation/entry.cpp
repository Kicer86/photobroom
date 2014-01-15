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


#include "entry.hpp"

#include <boost/crc.hpp>

#include "ifs.hpp"

namespace Database
{

    Entry::Entry(const std::shared_ptr<IStreamFactory>& stream): m_d(new Entry::Data(stream))
    {

    }


    Entry::Entry(Entry&& other): m_d(std::move(other.m_d))
    {
        this->operator=(std::move(other));
    }


    Entry::Entry(const APhotoInfo::Ptr& ptr, const std::shared_ptr<IStreamFactory>& stream):
        m_d(new Entry::Data(stream, ptr))
    {

    }


    Entry::~Entry()
    {
    }


    Entry& Entry::operator=(Entry && other)
    {
        m_d = std::move(other.m_d);
        other.m_d = nullptr;

        return *this;
    }


    bool Entry::operator==(const Entry&) const
    {
        return false;
    }


    Entry::crc32 Entry::calcCrc(const std::string &path) const
    {
        const int MAX_SIZE = 65536;
        boost::crc_32_type crc;
        std::shared_ptr<std::iostream> input = m_d->m_stream->openStream(path, std::ios_base::in | std::ios_base::binary);

        if (input != nullptr)
            do
            {
                char buf[MAX_SIZE];

                input->read(buf, MAX_SIZE);
                crc.process_bytes(buf, input->gcount());
            }
            while(input->fail() == false);

        const Entry::crc32 sum = crc();

        return sum;
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////


    Entry::Data::Data(const std::shared_ptr<IStreamFactory> &stream, const APhotoInfo::Ptr &photoInfo):
        m_crc(0xffffffff),
        m_path("null"),
        m_photoInfo(photoInfo),
        m_stream(stream)
    {

    }


    Entry::Data::Data(const std::shared_ptr<IStreamFactory> &stream): Data(stream, nullptr)
    {

    }


    Entry::Data::Data(): Data(nullptr, nullptr)
    {

    }



}
