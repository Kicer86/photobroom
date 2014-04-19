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

#ifndef PLUGIN_LOADER_HPP
#define PLUGIN_LOADER_HPP

#include <memory>

#include "sql_backend_base_export.h"

namespace Database
{
    class IPlugin;

    class SQL_BACKEND_BASE_EXPORT PluginLoader
    {
        public:
            PluginLoader();
            PluginLoader(const PluginLoader& other) = delete;
            ~PluginLoader();

            PluginLoader& operator=(const PluginLoader& other) = delete;

            std::unique_ptr<Database::IPlugin> get();

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;
    };

}

#endif // PLUGIN_LOADER_HPP
