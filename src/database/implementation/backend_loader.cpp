/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#include "backend_loader.hpp"

#include <iostream>

#include <QDir>
#include <QPluginLoader>

#include <system/filesystem.hpp>
#include <database/idatabase.hpp>


namespace Database
{

    namespace
    {
        struct PluginFinder final
        {
            PluginFinder(): m_plugins(), m_found(false) {}
            ~PluginFinder() {}

            void find()
            {
                if (!m_found)
                {
                    QDir pluginsDir = FileSystem::getPluginsPath();

                    pluginsDir.cd("database");
                    m_plugins = pluginsDir.entryInfoList(QStringList(), QDir::Files);

                    m_found = true;

                    for(const QFileInfo& info: m_plugins)
                        std::cout << "Found database plugin: " << info.absoluteFilePath().toStdString() << std::endl;
                }
            }

            Database::IBackend* load()
            {
                find();

                Database::IBackend* result = nullptr;

                //TODO: load plugins basing on config etc
                for(const QFileInfo& info: m_plugins)
                {
                    const QString path = info.absoluteFilePath();
                    if (path.contains("sqlite"))
                    {
                        QObject* rawPlugin = load(path);
                        result = dynamic_cast<Database::IBackend *>(rawPlugin);

                        break;
                    }
                }

                return result;
            }

            QObject* load(const QString& path)
            {
                std::cout << "Loading database plugin: " << path.toStdString() << std::endl;
                QPluginLoader loader(path);
                QObject* plugin = loader.instance();

                if (plugin == nullptr)
                    std::cerr << "\tError: " << loader.errorString().toStdString() << std::endl;

                return plugin;
            }

            QFileInfoList m_plugins;
            bool m_found;
        };


    }

    struct BackendBuilder::Impl
    {
        PluginFinder m_finder;
    };


    BackendBuilder::BackendBuilder(): m_impl(new Impl)
    {

    }


    BackendBuilder::~BackendBuilder()
    {

    }


    std::unique_ptr<Database::IBackend> BackendBuilder::get()
    {
        std::unique_ptr<Database::IBackend> result(m_impl->m_finder.load());

        return result;
    }

}
