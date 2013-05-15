/*
    Database for photos
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


#include "photosdatabase.hpp"

#include <unordered_map>
#include <string>

#include <boost/crc.hpp>

#include "entry.hpp"
#include "ifs.hpp"

#define MAX_SIZE 65536

struct PhotosDatabase::Impl
{

    Impl(Database::IConfiguration *config, const std::shared_ptr<FS> &stream): m_configuration(config), m_stream(stream)
    {
    }

    virtual ~Impl()
    {
    }

    Impl(const PhotosDatabase::Impl &) {}

    Impl& operator=(const Impl &)
    {
        return *this;
    }

    void add(const std::string &path, const Database::Description &description)
    {
        boost::crc_32_type crc;
        Entry entry;

        std::iostream *input = m_stream->openStream(path, std::ios_base::in | std::ios_base::binary);

        if (input != nullptr)
            do
            {
                char buf[MAX_SIZE];

                input->read(buf, MAX_SIZE);
                crc.process_bytes(buf, input->gcount());
            }
            while(input->fail() == false);

        entry.m_d->m_crc = crc();
        entry.m_d->m_path = decoratePath(path);

        m_db[entry.m_d->m_crc] = std::move(entry);
    }

    std::string decoratePath(const std::string &path) const
    {
        return std::string("file://") + path;
    }

    std::unordered_map<Entry::crc32, Entry> m_db;           //files managed by database

    Database::IConfiguration *m_configuration;
    std::shared_ptr<FS> m_stream;
};


PhotosDatabase::PhotosDatabase(Database::IConfiguration *config, const std::shared_ptr<FS> &stream): m_impl(new Impl(config, stream) )
{

}


PhotosDatabase::~PhotosDatabase()
{

}


bool PhotosDatabase::addFile(const std::string &path, const Database::Description &desc)
{


    return true;
}
