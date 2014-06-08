
#ifndef DATABASE_SQLITE_BACKEND_HPP
#define DATABASE_SQLITE_BACKEND_HPP

#include <memory>

#include <QObject>
#include <QtPlugin>

#include <database/idatabase.hpp>
#include <database/idatabase_plugin.hpp>
#include <database/sql_backends/sql_backend.hpp>
#include <database/sql_backends/generic_sql_query_constructor.hpp>
#include <database/implementation/ibackend_qt_interface.hpp>

#include "database_sqlite_backend_export.h"

namespace Database
{

    class SQLiteBackend final: public ASqlBackend, GenericSqlQueryConstructor
    {
        public:
            SQLiteBackend();
            virtual ~SQLiteBackend();

        private:
            // ASqlBackend:
            virtual bool prepareDB(QSqlDatabase *, const char *) override;
            virtual QString prepareFindTableQuery(const QString &) const override;
            virtual QString prepareColumnDescription(const ColDefinition&) const override;
            virtual ISqlQueryConstructor* getQueryConstructor() override;

            //ISqlQueryConstructor:
            virtual SqlQuery insertOrUpdate(const InsertQueryData&);

            struct Data;
            std::unique_ptr<Data> m_data;
    };

    
    class DATABASE_SQLITE_BACKEND_EXPORT SQLitePlugin final: public QObject, public IPlugin
    {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID DatabasePluginInterface_iid FILE "sqlite_backend.json")
            Q_INTERFACES(Database::IPlugin)   //'Database' namespace is obligatory

        public:
            SQLitePlugin();
            virtual ~SQLitePlugin();

            virtual std::unique_ptr<IBackend> constructBackend();
    };

}

#endif
