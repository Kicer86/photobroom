
/*
    Interface which needs to be implemented by database plugins.

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

#ifndef DATABASE_PLUGIN_HPP
#define DATABASE_PLUGIN_HPP

#include <memory>

#include <QString>

#define DatabasePluginInterface_iid "org.broom.plugins.database.iplugin"

namespace Database
{
    struct IBackend;

    struct IPlugin
    {
        //data for prj file
        struct PrjData
        {
            QString backendName;
            QString location;
        };

        virtual ~IPlugin() {}
        
        virtual std::unique_ptr<IBackend> constructBackend() = 0;
        virtual QString backendName() const = 0;                          
        virtual PrjData initPrjDir(const QString &) const = 0;            //prepares database in provided directory
    };
}

#endif
