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


#include "database_builder.hpp"

#include <assert.h>

#include <memory>
#include <fstream>
#include <map>

#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>
#include <configuration/constants.hpp>
#include <core/plugin_loader.hpp>
#include <database_tools/photos_analyzer.hpp>

#include "ifs.hpp"
#include "iphoto_info_creator.hpp"
#include "database_thread.hpp"
#include "photo_info_manager.hpp"
#include "idatabase_plugin.hpp"
#include "idatabase.hpp"
#include "implementation/photo_info.hpp"

//TODO: cleanup this file!

namespace Database
{

    const char* databaseLocation = "Database::Backend::DataLocation";

    struct Builder::Impl
    {
        Impl(const Impl &) = delete;

        Impl& operator=(const Impl &) = delete;

        //backend type
        enum Type
        {
            Main,
        };

        struct DatabaseObjects
        {
			DatabaseObjects(const std::shared_ptr<IDatabase> &database, 
							const std::shared_ptr<IBackend> &backend,
							const std::shared_ptr<IPhotoInfoManager> &manager,
                            const std::shared_ptr<PhotosAnalyzer> &analyzer
                           ) : m_database(database), m_backend(backend), m_photoManager(manager), m_photosAnalyzer(analyzer)
            {

            }
            
			~DatabaseObjects() {}
			//DatabaseObjects(const DatabaseObjects &) = delete;
			//DatabaseObjects& operator=(const DatabaseObjects &) = delete;

			std::shared_ptr<IDatabase> m_database;
			std::shared_ptr<IBackend> m_backend;
			std::shared_ptr<IPhotoInfoManager> m_photoManager;
            std::shared_ptr<PhotosAnalyzer> m_photosAnalyzer;
        };

        struct PhotoInfoCreator: Database::IPhotoInfoCreator
        {
            virtual IPhotoInfo::Ptr construct(const QString& path)
            {
                auto result = std::make_shared<PhotoInfo>(path);
                result->markStagingArea(true);                                //by default all new photos go to staging area.

                return result;
            }
        };

        std::map<ProjectInfo, DatabaseObjects> m_backends;
        IPluginLoader* pluginLoader;
        IConfiguration* m_configuration;
        std::shared_ptr<IBackend> defaultBackend;
        PhotoInfoCreator photoInfoCreator;

        Impl(): m_backends(), pluginLoader(nullptr), m_configuration(nullptr), defaultBackend(), photoInfoCreator()
        {}

    };


    Builder::Builder(): m_impl(new Impl)
    {

    }


    Builder::~Builder()
    {

    }


    void Builder::set(IPluginLoader* pluginLoader)
    {
        m_impl->pluginLoader = pluginLoader;
    }


    void Builder::set(IConfiguration* configuration)
    {
        m_impl->m_configuration = configuration;

        Optional<Configuration::EntryData> entry = m_impl->m_configuration->findEntry(Configuration::Constants::configLocation);

        assert(entry.is_initialized());

        const QString configPath = entry->value();
        const QString dbPath = configPath + "/database";

        const QString configuration_xml =
        "<configuration>                                        "
        "   <keys>                                              "
        "   </keys>                                             "
        "                                                       "
        "   <defaults>                                          "
        "       <key name='Database::Backend::DataLocation' value='" + dbPath + "' />"
        "   </defaults>                                         "
        "</configuration>                                       ";

        configuration->registerXml(configuration_xml);
    }


    IDatabase* Builder::get(const ProjectInfo& info)
    {
        auto backendIt = m_impl->m_backends.find(info);

        if (backendIt == m_impl->m_backends.end())
        {
            Database::IPlugin* plugin = m_impl->pluginLoader->getDBPlugin(info.backendName);
            assert(plugin);

            std::shared_ptr<PhotoInfoManager> manager(new PhotoInfoManager);
            std::shared_ptr<IBackend> backend = plugin->constructBackend();
            std::shared_ptr<IDatabase> database(new DatabaseThread(backend.get()));
            std::shared_ptr<PhotosAnalyzer> analyzer(new PhotosAnalyzer);

            backend->setPhotoInfoManager(manager.get());
            backend->setPhotoInfoCreator(&m_impl->photoInfoCreator);
            manager->setDatabase(database.get());
            analyzer->setDatabase(database.get());

            Database::Task task = database->prepareTask(nullptr);

            const bool status = database->init(task, info.databaseLocation);

            if (status)
            {
                Impl::DatabaseObjects dbObjs(database, backend, manager, analyzer);
                auto insertIt = m_impl->m_backends.emplace(std::make_pair(info, dbObjs));
                backendIt = insertIt.first;
            }
        }

        return backendIt->second.m_database.get();
    }


    void Builder::closeAll()
    {
        for(auto& backend: m_impl->m_backends)
            backend.second.m_database->closeConnections();
    }

}
