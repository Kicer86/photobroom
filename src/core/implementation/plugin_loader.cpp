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

#include <iostream>
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
        PluginFinder(): m_db_plugins(), m_loaded_plugins(), m_found(false)
        {

        }

        ~PluginFinder()
        {
            for(auto plugin: m_loaded_plugins)
                delete plugin;
        }

        void find()
        {
            if (!m_found)
            {
                QDir pluginsDir = FileSystem::getPluginsPath();

                pluginsDir.cd("database");
                m_db_plugins = pluginsDir.entryInfoList(QStringList(), QDir::Files);

                m_found = true;

                for(const QFileInfo& info: m_db_plugins)
                    std::cout << "Found database plugin: " << info.absoluteFilePath().toStdString() << std::endl;
            }
        }

        QObject* loadDBPlugin(const QString& name)
        {
            find();

            QObject* result = nullptr;

            //TODO: load plugins basing on config etc
            for(const QFileInfo& info: m_db_plugins)
            {
                const QString path = info.absoluteFilePath();
                const QString pluginName = info.fileName();

                if (pluginName.contains(name))
                {
                    result = load(path);

                    break;
                }
            }

            return result;
        }

    private:
        QObject* load(const QString& path)
        {
            std::cout << "Loading database plugin: " << path.toStdString() << std::endl;
            QPluginLoader loader(path);
            QObject* plugin = loader.instance();

            if (plugin)
                m_loaded_plugins.push_back(plugin);
            else
                std::cerr << "\tError: " << loader.errorString().toStdString() << std::endl;

            return plugin;
        }

        QFileInfoList m_db_plugins;
        std::deque<QObject *> m_loaded_plugins;
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


Database::IPlugin* PluginLoader::getDBPlugin(const QString& name)
{
    QObject* raw_result = m_impl->m_finder.loadDBPlugin(name);
    Database::IPlugin* result = dynamic_cast<Database::IPlugin *>(raw_result);
    
    assert(result != nullptr);

    return result;
}
