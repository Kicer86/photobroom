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

#include <cassert>
#include <fstream>
#include <map>
#include <memory>

#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>
#include <configuration/constants.hpp>
#include <core/plugin_loader.hpp>
#include <database_tools/photos_analyzer.hpp>

#include "ifs.hpp"
#include "database_thread.hpp"
#include "photo_info_cache.hpp"
#include "idatabase_plugin.hpp"
#include "idatabase.hpp"
#include "ibackend.hpp"
#include "implementation/photo_info.hpp"
#include "project_info.hpp"

//TODO: cleanup this file!

namespace Database
{

    const char* databaseLocation = "Database::Backend::DataLocation";

    struct InitTask: Database::IInitTask
    {
        virtual ~InitTask() {}
        virtual void got(bool status)
        {
            assert(status);
        }
    };

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
            DatabaseObjects() : m_database(), m_backend(), m_photoManager(), m_photosAnalyzer() {}
            ~DatabaseObjects() {}

            DatabaseObjects(DatabaseObjects&& other):
                m_database(std::move(other.m_database)),
                m_backend(std::move(other.m_backend)),
                m_photoManager(std::move(other.m_photoManager)),
                m_photosAnalyzer(std::move(other.m_photosAnalyzer))
            {

            }

            std::unique_ptr<IDatabase> m_database;
            std::unique_ptr<IBackend> m_backend;
            std::unique_ptr<IPhotoInfoCache> m_photoManager;
            std::unique_ptr<PhotosAnalyzer> m_photosAnalyzer;
        };

        std::map<ProjectInfo, DatabaseObjects> m_backends;
        IPluginLoader* pluginLoader;
        IConfiguration* m_configuration;
        std::shared_ptr<IBackend> defaultBackend;
        ILogger* m_logger;
        ITaskExecutor* m_task_executor;

        Impl(): m_backends(), pluginLoader(nullptr), m_configuration(nullptr), defaultBackend(), m_logger(nullptr), m_task_executor(nullptr)
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

        /*
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
        */
    }


    void Builder::set(ILogger* logger)
    {
        m_impl->m_logger = logger;
    }


    void Builder::set(ITaskExecutor* taskExecutor)
    {
        m_impl->m_task_executor = taskExecutor;
    }


    IDatabase* Builder::get(const ProjectInfo& info)
    {
        auto backendIt = m_impl->m_backends.find(info);

        if (backendIt == m_impl->m_backends.end())
        {
            Database::IPlugin* plugin = m_impl->pluginLoader->getDBPlugin(info.backendName);
            assert(plugin);

            PhotoInfoCache* cache = new PhotoInfoCache;
            std::unique_ptr<IBackend> backend = plugin->constructBackend();
            IDatabase* database = new DatabaseThread(backend.get());
            PhotosAnalyzer* analyzer = new PhotosAnalyzer;

            backend->setPhotoInfoManager(cache);
            backend->set(m_impl->m_logger);
            cache->setDatabase(database);
            analyzer->setDatabase(database);
            analyzer->set(m_impl->m_task_executor);
            analyzer->set(m_impl->m_configuration);

            std::unique_ptr<Database::IInitTask> task(new InitTask);

            const bool status = database->exec(std::move(task), info);

            if (status)
            {
                Impl::DatabaseObjects dbObjs;
                dbObjs.m_backend = std::move(backend);
                dbObjs.m_database.reset(database);
                dbObjs.m_photoManager.reset(cache);
                dbObjs.m_photosAnalyzer.reset(analyzer);

                auto insertIt = m_impl->m_backends.insert(std::make_pair(info, std::move(dbObjs)));
                backendIt = insertIt.first;
            }
        }

        return backendIt->second.m_database.get();
    }


    void Builder::closeAll()
    {
        for (auto& backend: m_impl->m_backends)
            backend.second.m_database->closeConnections();
    }

}
