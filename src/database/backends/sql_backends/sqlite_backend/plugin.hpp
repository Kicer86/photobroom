
#pragma once

#include <memory>

#include <QObject>
#include <QtPlugin>

#include <database/idatabase_plugin.hpp>
#include <database/project_info.hpp>
#include <database/implementation/ibackend_qt_interface.hpp>

#include "database_sqlite_plugin_export.h"


namespace Database
{
    class DATABASE_SQLITE_PLUGIN_EXPORT SQLitePlugin final: public IPlugin
    {
            Q_OBJECT

#ifndef STATIC_PLUGINS
            Q_PLUGIN_METADATA(IID DatabasePluginInterface_iid FILE "sqlite_backend.json")
            Q_INTERFACES(Database::IPlugin)    //'Database' namespace is obligatory
#endif

        public:
            SQLitePlugin();
            virtual ~SQLitePlugin();

            virtual std::unique_ptr<IBackend> constructBackend(IConfiguration *, ILogger *) override;
            virtual QString backendName() const override;
            virtual ProjectInfo initPrjDir(const QString& dir, const QString& name) const override;
            virtual QLayout* buildDBOptions() override;
            virtual char simplicity() const override;
    };

}
