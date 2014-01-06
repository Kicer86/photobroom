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


#ifndef MEMORYDATABASE_HPP
#define MEMORYDATABASE_HPP

#include <memory>
#include <fstream>

#include "idatabase.hpp"

#include "database_export.h"

struct IStreamFactory;

namespace Database
{
    struct IConfiguration;

    class DATABASE_EXPORT MemoryDatabase final: public Database::IFrontend
    {
        public:
            struct Impl;

            MemoryDatabase(const std::shared_ptr<IStreamFactory> &);
            virtual ~MemoryDatabase();

            virtual bool addPhoto(const APhotoInfo::Ptr &) override;
            virtual void setBackend(const std::shared_ptr<IBackend> &) override;

            virtual void close() override;

        private:
            std::unique_ptr<Impl> m_impl;
    };

}
#endif // MEMORYDATABASE_HPP
