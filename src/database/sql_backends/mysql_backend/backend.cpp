
#include "backend.hpp"

#include <stdexcept>

#include <QProcess>
#include <QSqlDatabase>
#include <QDir>

#include <configuration/configurationfactory.hpp>
#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>
#include <database/database_builder.hpp>
#include <sql_backends/table_definition.hpp>

#include "mysql_server.hpp"


namespace Database
{

    struct MySqlBackend::Data
    {
        Data(): m_server(), m_dbName(nullptr), m_initialized(false) {}
        Data(const Data &) = delete;
        Data& operator=(const Data &) = delete;

        ~Data() { }

        MySqlServer m_server;
        const char* m_dbName;
        bool m_initialized;
    };


    MySqlBackend::MySqlBackend(): m_data(new Data)
    {

    }


    MySqlBackend::~MySqlBackend()
    {
        //TODO: assert for db closed. In needs to be closed before MySQL server termination
    }


    bool MySqlBackend::prepareDB(QSqlDatabase* db, const char* name)
    {
        bool status = true;

        if (m_data->m_initialized == false)
        {
            m_data->m_dbName = name;
            auto entry = ConfigurationFactory::get()->findEntry(Database::databaseLocation);

            //create base directory
            if (entry)
            {
                QString storagePath(entry->value().c_str());
                storagePath += "/MySQL";

                if (QDir().exists(storagePath) == false)
                    status = QDir().mkpath(storagePath);

                if (status)
                {
                    //start mysql process
                    storagePath += "/";

                    const QString socketPath = m_data->m_server.run_server(storagePath);

                    if (socketPath.isEmpty() == false)
                    {
                        QSqlDatabase db_obj;
                        //setup db connection
                        db_obj = QSqlDatabase::addDatabase("QMYSQL", name);
                        db_obj.setConnectOptions("UNIX_SOCKET=" + socketPath);
                        //db_obj.setDatabaseName("broom");
                        db_obj.setHostName("localhost");
                        db_obj.setUserName("root");

                        *db = db_obj;
                    }

                    m_data->m_initialized = socketPath.isEmpty() == false;;
                }
            }
        }

        return status;
    }


    bool MySqlBackend::onAfterOpen()
    {
        return ASqlBackend::createDB(m_data->m_dbName);
    }


    QString MySqlBackend::prepareCreationQuery(const QString& name, const QString& columns) const
    {
        //Here we force InnoDB engine which may be default
        //http://dev.mysql.com/doc/refman/5.5/en/innodb-default-se.html

        const QString result = QString("CREATE TABLE %1(%2) ENGINE=InnoDB;").arg(name).arg(columns);

        return result;
    }


    QString MySqlBackend::prepareColumnDescription(const ColDefinition& col) const
    {
        QString result;

        switch(col.type)
        {
            case ColDefinition::Type::Regular:
                result = col.name;
                break;

            case ColDefinition::Type::ID:
                result = col.name + " " + "UNSIGNED INT PRIMARY KEY AUTO_INCREMENT";
                break;
        }

        return result;
    }


    MySqlPlugin::MySqlPlugin(): QObject()
    {

    }


    MySqlPlugin::~MySqlPlugin()
    {

    }


    std::unique_ptr<IBackend> MySqlPlugin::constructBackend()
    {
        return std::unique_ptr<IBackend>(new MySqlBackend);
    }


    const ISqlQueryConstructor* MySqlBackend::getQueryConstructor() const
    {
        return this;
    }


    SqlQuery MySqlBackend::insertOrUpdate(const InsertQueryData &) const
    {
        assert(!"not implemented");

        return SqlQuery();
    }

}
