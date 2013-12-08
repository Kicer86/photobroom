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
#include "iconfiguration.hpp"
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
            
            struct Config: public Database::IConfiguration
            {
                virtual ~Config() {}

                virtual std::string getLocation() const
                {
                    std::string result;
#ifdef OS_UNIX
                    const char *home = getenv("HOME");
                    result = std::string(home) + "/.config/broom/";
#elif OS_WIN
                    const char *home = getenv("LOCALAPPDATA");
                    result = std::string(home) + "/broom/";
#else
    #error unknown os
#endif
                    assert(result != "");
                    return result;
                }
            };

            struct FSImpl: public IStreamFactory
            {
                virtual ~FSImpl() 
                {
                    
                }

                virtual std::shared_ptr<std::iostream> openStream(const std::string &filename,
                                                  std::ios_base::openmode mode) override
                {
                    auto stream = std::make_shared<std::fstream>();

                    stream->open(filename.c_str(), mode);

                    return stream;
                }
            };
            
            Config *config = new Config;
            std::shared_ptr<IStreamFactory> fs(new FSImpl);
            IFrontend *frontend = new MemoryDatabase(config, fs);
            
            defaultDatabase.reset(frontend);
            defaultDatabase->setBackend(std::shared_ptr<Database::IBackend>(new Database::PrimitiveBackend));
        }

        return defaultDatabase.get();
    }

}
