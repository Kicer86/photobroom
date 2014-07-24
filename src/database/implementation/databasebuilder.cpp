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
#include <map>

#include <configuration/configurationfactory.hpp>
#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>

#include "ifs.hpp"
#include "plugin_loader.hpp"
#include "database_thread.hpp"
#include "idatabase_plugin.hpp"
#include "idatabase.hpp"

//TODO: cleanup this file!

namespace Database
{

    const char* databaseLocation = "Database::Backend::DataLocation";

    namespace
    {

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

        //class which initializes configuration with db entries
        struct ConfigurationInitializer: public Configuration::IInitializer
        {
            ConfigurationInitializer()
            {

            }

            virtual std::string getXml()
            {
                std::shared_ptr< ::IConfiguration > config = ConfigurationFactory::get();
                Optional<Configuration::EntryData> entry = config->findEntry(Configuration::configLocation);

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
        };
    }

    struct Builder::Impl
    {
        std::unique_ptr<IPlugin> plugin;
        std::shared_ptr<IBackend> defaultBackend;
        PluginLoader backendBuilder;
        ConfigurationInitializer configInitializer;

        //bavkend type
        enum Type
        {
            Main,
        };

        std::map<Type, std::unique_ptr<IDatabase>> m_backends;

        Impl(): plugin(), defaultBackend(), backendBuilder(), configInitializer(), m_backends()
        {}

        IPlugin* getPlugin()
        {
            if (plugin.get() == nullptr)
                plugin = backendBuilder.get();

            return plugin.get();
        }
    };


    Builder::Builder(): m_impl(new Impl)
    {

    }


    Builder::~Builder()
    {

    }


    Builder* Builder::instance()
    {
        static Builder builder;
        return &builder;
    }


    void Builder::initConfig()
    {
        std::shared_ptr< ::IConfiguration > config = ConfigurationFactory::get();
        config->registerInitializer(&m_impl->configInitializer);
    }


    IDatabase* Builder::get()
    {
        const char* dbType = "broom";

        auto backendIt = m_impl->m_backends.find(Impl::Main);

        if (backendIt == m_impl->m_backends.end())
        {
            std::unique_ptr<IDatabase> database(new AsynchronousDatabase(m_impl->getPlugin()->constructBackend()));
            Database::Task task = database->prepareTask(nullptr);

            const bool status = database->init(task, dbType);

            if (status)
            {
                auto insertIt = m_impl->m_backends.emplace( std::make_pair(Impl::Main, std::move(database)) );
                backendIt = insertIt.first;
            }
        }

        return backendIt->second.get();
    }


    void Builder::closeAll()
    {
        for(auto& backend: m_impl->m_backends)
            backend.second->closeConnections();
    }

}
