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


#ifndef PHOTOSDATABASE_HPP
#define PHOTOSDATABASE_HPP

#include <memory>
#include <fstream>

#include "idatabase.hpp"

struct FS;

namespace Database
{
    struct IConfiguration;
   
    class PhotosDatabase: public Database::IFrontend
    {
        public:
            PhotosDatabase(Database::IConfiguration *config, const std::shared_ptr<FS> &);
            virtual ~PhotosDatabase();

            virtual bool addFile(const std::string &path, const IFrontend::Description &);
            virtual void setBackend(IBackend *);

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;
    };

}
#endif // PHOTOSDATABASE_HPP
