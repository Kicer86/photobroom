/*
 * Plugins loader class
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "plugin_loader.hpp"
#include <ilogger.hpp>

#include <deque>
#include <cassert>

#include <QDir>
#include <QPluginLoader>

#include <system/filesystem.hpp>
#include <database/idatabase_plugin.hpp>


namespace
{
    struct PluginFinder final
    {
        PluginFinder(): m_logger(nullptr), m_db_plugins(), m_found(false)
        {

        }

        PluginFinder(const PluginFinder &) = delete;

        ~PluginFinder()
        {
            for(auto plugin: m_db_plugins)
                delete plugin;
        }

        PluginFinder& operator=(const PluginFinder &) = delete;
        
        void set(ILogger* logger)
        {
            m_logger = logger;
        }

        void find_all_db_plugins()
        {
            if (!m_found)
            {
                QDir pluginsDir = FileSystem::getPluginsPath();

                pluginsDir.cd("database");
                QFileInfoList db_plugins = pluginsDir.entryInfoList(QStringList(), QDir::Files);

                m_found = true;

                for(const QFileInfo& info: db_plugins)
                {
                    const QString path = info.absoluteFilePath();
                                    
                    m_logger->log("PluginLoader", ILogger::Severity::Info, "Found database plugin: " + path.toStdString());
                    
                    QObject* raw_plugin = load(path);
                    Database::IPlugin* plugin = dynamic_cast<Database::IPlugin *>(raw_plugin);
                    
                    m_db_plugins.push_back(plugin);
                }
            }
        }

        Database::IPlugin* loadDBPlugin(const QString& name)
        {
            find_all_db_plugins();

            Database::IPlugin* result = nullptr;

            //TODO: load plugins basing on config etc
            for(Database::IPlugin* plugin: m_db_plugins)
            {
                if (plugin->backendName() == name)
                {
                    result = plugin;
                    break;
                }
            }

            return result;
        }

        const std::deque<Database::IPlugin *>& getDBPlugins()
        {
            find_all_db_plugins();

            return m_db_plugins;
        }

    private:
        QObject* load(const QString& path)
        {
            m_logger->log("PluginLoader", ILogger::Severity::Info, "Loading database plugin: " + path.toStdString());
            QPluginLoader loader(path);
            QObject* plugin = loader.instance();

            if (plugin == nullptr)
                m_logger->log("PluginLoader", ILogger::Severity::Error, "\tError: " + loader.errorString().toStdString());

            return plugin;
        }

        ILogger* m_logger;
        std::deque<Database::IPlugin *> m_db_plugins;
        bool m_found;
    };


}

struct PluginLoader::Impl
{
    Impl(): m_finder() {}

    PluginFinder m_finder;
};


PluginLoader::PluginLoader(): m_impl(new Impl)
{

}


PluginLoader::~PluginLoader()
{

}


void PluginLoader::set(ILogger* logger)
{
    m_impl->m_finder.set(logger);
}


Database::IPlugin* PluginLoader::getDBPlugin(const QString& name)
{
    Database::IPlugin* result = m_impl->m_finder.loadDBPlugin(name);
    
    return result;
}


const std::deque<Database::IPlugin *>& PluginLoader::getDBPlugins() const
{
    return m_impl->m_finder.getDBPlugins();
}

