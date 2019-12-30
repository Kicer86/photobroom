/*
 * Photo Broom - photos management tool.
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

#ifndef PLUGIN_LOADER_HPP
#define PLUGIN_LOADER_HPP

#include <memory>

#include "iplugin_loader.hpp"

struct ILoggerFactory;

class PluginLoader: public IPluginLoader
{
    public:
        PluginLoader();
        PluginLoader(const PluginLoader& other) = delete;
        virtual ~PluginLoader();

        PluginLoader& operator=(const PluginLoader& other) = delete;
        
        void set(ILoggerFactory *);

        Database::IPlugin* getDBPlugin(const QString &) override;
        const std::vector<Database::IPlugin *>& getDBPlugins() const override;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};


#endif // PLUGIN_LOADER_HPP
