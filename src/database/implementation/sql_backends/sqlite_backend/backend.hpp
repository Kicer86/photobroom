
#ifndef DATABASE_SQLITE_BACKEND_HPP
#define DATABASE_SQLITE_BACKEND_HPP

#include <memory>

#include <QObject>
#include <QtPlugin>

#include <database/idatabase.hpp>
#include <database/implementation/sql_backends/sql_backend.hpp>
#include <database/implementation/ibackend_qt_interface.hpp>

#include "database_sqlite_backend_export.h"

namespace Database
{

    class DATABASE_SQLITE_BACKEND_EXPORT SQLiteBackend final: public QObject, public ASqlBackend
    {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID BackendInterface_iid FILE "sqlite_backend.json")
            Q_INTERFACES(Database::IBackend)   //usage base interface. 'Database' namespace is obligatory

        public:
            SQLiteBackend();
            virtual ~SQLiteBackend();

            virtual bool prepareDB(QSqlDatabase*) override;
            virtual QString prepareCreationQuery(const QString& name, const QString& columns) const override;

        private:
            struct Data;
            std::unique_ptr<Data> m_data;
    };

}

#endif
