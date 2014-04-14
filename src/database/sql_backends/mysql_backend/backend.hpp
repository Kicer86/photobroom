
#ifndef DATABASE_MYSQL_BACKEND_HPP
#define DATABASE_MYSQL_BACKEND_HPP


#include <memory>

#include <database/idatabase.hpp>
#include <database/sql_backends/sql_backend.hpp>
#include <database/implementation/ibackend_qt_interface.hpp>

#include "database_mysql_backend_export.h"

namespace Database
{

    class DATABASE_MYSQL_BACKEND_EXPORT MySqlBackend final: public QObject, public ASqlBackend
    {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID BackendInterface_iid FILE "mysql_backend.json")
            Q_INTERFACES(Database::IBackend)   //usage base interface. 'Database' namespace is obligatory


        public:
            MySqlBackend();
            virtual ~MySqlBackend();

        private:
            virtual bool prepareDB(QSqlDatabase*) override;
            virtual QString prepareCreationQuery(const QString& name, const QString& columns) const override;
            virtual QString prepareColumnDescription(const ColDefinition&) const override;
            virtual bool onAfterOpen() override;

            struct Data;
            std::unique_ptr<Data> m_data;
    };

}

#endif
