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

#include "ifs.hpp"
#include "database_thread.hpp"
#include "photo_info_cache.hpp"
#include "idatabase_plugin.hpp"
#include "idatabase.hpp"
#include "ibackend.hpp"
#include "backends/photo_info.hpp"
#include "photo_info_storekeeper.hpp"
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

        struct DatabaseObjects: Database::IDBPack
        {
            DatabaseObjects() : m_database(), m_backend(), m_cache(), m_storekeeper() {}

            DatabaseObjects(DatabaseObjects&& other):
                m_database(std::move(other.m_database)),
                m_backend(std::move(other.m_backend)),
                m_cache(std::move(other.m_cache)),
                m_storekeeper(std::move(other.m_storekeeper))
            {

            }

            ~DatabaseObjects()
            {
                //destroy objects in right order
                m_storekeeper.reset();
                m_cache.reset();
                m_database.reset();
                m_backend.reset();
            }

            std::unique_ptr<IDatabase> m_database;
            std::unique_ptr<IBackend> m_backend;
            std::unique_ptr<IPhotoInfoCache> m_cache;
            std::unique_ptr<PhotoInfoStorekeeper> m_storekeeper;

            IDatabase* get() override
            {
                return m_database.get();
            }
        };

        IPluginLoader* pluginLoader;
        ILogger* m_logger;

        Impl(): pluginLoader(nullptr), m_logger(nullptr)
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


    void Builder::set(ILogger* logger)
    {
        m_impl->m_logger = logger;
    }


    std::unique_ptr<IDBPack> Builder::get(const ProjectInfo& info)
    {
        Database::IPlugin* plugin = m_impl->pluginLoader->getDBPlugin(info.backendName);
        assert(plugin);

        PhotoInfoCache* cache = new PhotoInfoCache;
        std::unique_ptr<IBackend> backend = plugin->constructBackend();
        IDatabase* database = new DatabaseThread(backend.get());
        PhotoInfoStorekeeper* storekeeper = new PhotoInfoStorekeeper;

        backend->setPhotoInfoCache(cache);
        backend->set(m_impl->m_logger);
        backend->addEventsObserver(storekeeper);
        cache->setDatabase(database);
        storekeeper->setDatabase(database);
        storekeeper->setCache(cache);

        std::unique_ptr<Database::IInitTask> task(new InitTask);

        const bool status = database->exec(std::move(task), info);

        std::unique_ptr<IDBPack> result;

        if (status)
        {
            Impl::DatabaseObjects* dbObjs = new Impl::DatabaseObjects;
            dbObjs->m_backend = std::move(backend);
            dbObjs->m_database.reset(database);
            dbObjs->m_cache.reset(cache);
            dbObjs->m_storekeeper.reset(storekeeper);

            result.reset(dbObjs);
        }

        return result;
    }

}
