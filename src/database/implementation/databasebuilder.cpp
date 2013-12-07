/*
    Factory for database
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


#include "databasebuilder.hpp"

#include <assert.h>

#include <memory>
#include <fcntl.h>

#include "memorydatabase.hpp"
#include "ifs.hpp"
#include "implementation/backend.hpp"

namespace Database
{

    namespace
    {
        std::unique_ptr<IFrontend> defaultDatabase;
    }


    Builder::Builder()
    {

    }


    Builder::~Builder()
    {

    }


    IFrontend* Builder::get()
    {
        if (defaultDatabase.get() == nullptr)
        {
            struct FSImpl: public FS
            {
                virtual ~FSImpl() 
                {
                    
                }

                virtual std::iostream* openStream(const std::string &filename,
                                                  std::ios_base::openmode mode)
                {
                    std::fstream *stream = new std::fstream;

                    stream->open(filename.c_str(), mode);

                    return stream;
                }

                virtual void closeStream(std::iostream *stream)
                {
                    assert(dynamic_cast<std::fstream *>(stream));

                    std::fstream *fstream = static_cast<std::fstream *>(stream);

                    fstream->close();
                    delete fstream;
                };
            };
            
            std::shared_ptr<FS> fs(new FSImpl);
            IFrontend *frontend = new MemoryDatabase(fs);
            
            defaultDatabase.reset(frontend);
            defaultDatabase->setBackend(std::shared_ptr<Database::IBackend>(new Database::PrimitiveBackend));
        }

        return defaultDatabase.get();
    }

}
