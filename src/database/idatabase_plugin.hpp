
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
#include "idatabase_builder.hpp"
#include "database_export.h"

#define DatabasePluginInterface_iid "org.broom.plugins.database.iplugin"

class QLayout;

namespace Database
{
    struct IBackend;

    struct DATABASE_EXPORT IPlugin: QObject
    {
        virtual ~IPlugin() {}

        virtual std::unique_ptr<IBackend> constructBackend(IConfiguration *, ILoggerFactory *) = 0;  //return backend object
        virtual QString backendName() const = 0;                                                     //return backend name
        virtual ProjectInfo initPrjDir(const QString& dir, const QString& name) const = 0;           //prepares database in provided directory
        virtual QLayout* buildDBOptions() = 0;                                                       //return QLayout for ProjectCreator dialog with options for specific backend
        virtual char simplicity() const = 0;                                                         //simplicity of backend. 127 for very user friendly, -128 for complex

        Q_OBJECT
    };
}

#endif
