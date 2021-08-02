
#ifndef DATABASE_MYSQL_BACKEND_HPP
#define DATABASE_MYSQL_BACKEND_HPP

#include <memory>

#include <database/idatabase.hpp>
#include <database/backends/sql_backends/sql_backend.hpp>
#include <database/backends/sql_backends/generic_sql_query_constructor.hpp>
#include <database/implementation/ibackend_qt_interface.hpp>

#include "database_mysql_backend_export.h"

namespace Database
{

    class MySqlBackend final: public ASqlBackend, GenericSqlQueryConstructor
    {
        public:
            MySqlBackend(IConfiguration *, ILogger *);
            virtual ~MySqlBackend();

        private:
            // ASqlBackend:
            virtual BackendStatus prepareDB(const ProjectInfo &) override;
            virtual const IGenericSqlQueryGenerator* getGenericQueryGenerator() const override;

            // GenericSqlQueryConstructor:
            virtual QString prepareCreationQuery(const QString& name, const QString& columns) const override;
            virtual QString getTypeFor(ColDefinition::Purpose) const override;

            struct Data;
            std::unique_ptr<Data> m_data;
    };


    class DATABASE_MYSQL_BACKEND_EXPORT MySqlPlugin final: public IPlugin
    {
            Q_OBJECT

#ifndef STATIC_PLUGINS
            Q_PLUGIN_METADATA(IID DatabasePluginInterface_iid FILE "mysql_backend.json")
            Q_INTERFACES(Database::IPlugin)    //'Database' namespace is obligatory
#endif

        public:
            MySqlPlugin();
            virtual ~MySqlPlugin();

            virtual std::unique_ptr<IBackend> constructBackend(IConfiguration *, ILogger *) override;
            virtual QString backendName() const override;
            virtual ProjectInfo initPrjDir(const QString &, const QString &) const override;
            virtual QLayout* buildDBOptions() override;
            virtual char simplicity() const override;
    };

}

#endif
