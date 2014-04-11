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
#include <fstream>

#include "configuration/configurationfactory.hpp"
#include "configuration/iconfiguration.hpp"
#include "configuration/entrydata.hpp"

#include "memorydatabase.hpp"
#include "ifs.hpp"
#include "backend_loader.hpp"

//TODO: cleanup this file!

namespace Database
{

    const char* databaseLocation = "Database::Backend::DataLocation";

    namespace
    {
        std::unique_ptr<IFrontend> defaultDatabase;
        std::shared_ptr<IBackend> defaultBackend;
        BackendBuilder backendBuilder;

        struct StreamFactory: public IStreamFactory
        {
            virtual ~StreamFactory()
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

        //object which initializes configuration with db entries
        struct ConfigurationInitializer: public Configuration::IInitializer
        {
            ConfigurationInitializer()
            {
                std::shared_ptr< ::IConfiguration > config = ConfigurationFactory::get();
                config->registerInitializer(this);
            }

            virtual std::string getXml()
            {
                std::shared_ptr< ::IConfiguration > config = ConfigurationFactory::get();
                boost::optional<Configuration::EntryData> entry = config->findEntry(Configuration::configLocation);

                assert(entry.is_initialized());

                const std::string configPath = entry->value();
                const std::string dbPath = configPath + "/database";

                const std::string configuration_xml =
                    "<configuration>                                        "
                    "   <keys>                                              "
                    "   </keys>                                             "
                    "                                                       "
                    "   <defaults>                                          "
                    "       <key name='Database::Backend::DataLocation' value='" + dbPath + "' />"
                    "   </defaults>                                         "
                    "</configuration>                                       ";

                return configuration_xml;
            }
        } initializer;
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
            std::shared_ptr<IStreamFactory> fs = std::make_shared<StreamFactory>();
            std::unique_ptr<IFrontend> frontend(new MemoryDatabase(fs));
            std::shared_ptr<Database::IBackend> backend = getBackend();

            if (backend.get() != nullptr)
            {
                defaultDatabase = std::move(frontend);
                defaultDatabase->setBackend(backend);
            }
            //else TODO: emit signal to signalize there are some problems at initialization
        }

        return defaultDatabase.get();
    }


    std::shared_ptr<IBackend> Builder::getBackend()
    {
        if (defaultBackend.get() == nullptr)
        {
            defaultBackend = backendBuilder.get();
            const bool status = defaultBackend->init();

            if (!status)
                defaultBackend.reset();
        }

        return defaultBackend;
    }


}
