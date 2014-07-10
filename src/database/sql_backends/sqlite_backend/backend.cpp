
#include "backend.hpp"

#include <stdexcept>

#include <QSqlDatabase>
#include <QStringList>
#include <QDir>

#include <configuration/configurationfactory.hpp>
#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>
#include <database/databasebuilder.hpp>
#include <sql_backends/table_definition.hpp>
#include <sql_backends/query_structs.hpp>

namespace Database
{

    struct SQLiteBackend::Data
    {
        Data(): m_initialized(false) {}

        ~Data()
        {

        }

        bool prepareDB(QSqlDatabase* db, const char* name)
        {
            bool status = true;

            if (m_initialized == false)
            {
                auto entry = ConfigurationFactory::get()->findEntry(Database::databaseLocation);

                //create base directory
                if (entry)
                {
                    QString storagePath(entry->value().c_str());

                    storagePath += "/SQLite";

                    if (QDir().exists(storagePath) == false)
                        status = QDir().mkpath(storagePath);

                    if (status)
                    {
                        storagePath += QString("/%1.db").arg(name);

                        QSqlDatabase db_obj;
                        //setup db connection
                        db_obj = QSqlDatabase::addDatabase("QSQLITE", name);
                        db_obj.setDatabaseName(storagePath);

                        *db = db_obj;
                    }
                }
            }

            return status;
        }

        bool m_initialized;
    };


    SQLiteBackend::SQLiteBackend(): m_data(new Data)
    {

    }


    SQLiteBackend::~SQLiteBackend()
    {

    }


    bool SQLiteBackend::prepareDB(QSqlDatabase* db, const char* name)
    {
        return m_data->prepareDB(db, name);
    }


    QString SQLiteBackend::prepareFindTableQuery(const QString& name) const
    {
        return QString("SELECT name FROM sqlite_master WHERE name='%1';").arg(name);
    }


    QString SQLiteBackend::prepareColumnDescription(const ColDefinition& col) const
    {
        QString result;

        switch(col.type)
        {
            case ColDefinition::Type::Regular:
                result = col.name;
                break;

            case ColDefinition::Type::ID:
                result = col.name + " " + "INTEGER PRIMARY KEY AUTOINCREMENT";
                break;
        }

        return result;
    }


    const ISqlQueryConstructor* SQLiteBackend::getQueryConstructor() const
    {
        return this;
    }


    SqlQuery SQLiteBackend::insertOrUpdate(const InsertQueryData& data) const
    {
        QString result("INSERT OR REPLACE INTO %1(%2) VALUES(%3)");

        result = result.arg(data.getName());
        result = result.arg(data.getColumns().join(", "));
        result = result.arg(data.getValues().join(", "));

        return result;

    }


    SQLitePlugin::SQLitePlugin(): QObject()
    {

    }


    SQLitePlugin::~SQLitePlugin()
    {

    }


    std::unique_ptr<IBackend> SQLitePlugin::constructBackend()
    {
        return std::unique_ptr<IBackend>(new SQLiteBackend);
    }

}
