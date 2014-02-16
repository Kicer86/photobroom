
#include "backend.hpp"

#include <stdexcept>

#include <QProcess>
#include <QSqlDatabase>

#include <boost/filesystem.hpp>

#include "configuration/configurationfactory.hpp"
#include "configuration/iconfiguration.hpp"
#include "configuration/entrydata.hpp"

#include "entry.hpp"
#include "databasebuilder.hpp"

#include "mysql_server.hpp"


namespace Database
{
    const char* QDatabaseName = "Backend";

    struct MySqlBackend::Data
    {
        Data(): m_initialized(false), m_server() {}

        ~Data()
        {

        }

        bool prepareDB(QSqlDatabase* db)
        {
            bool status = true;

            if (m_initialized == false)
            {
                auto entry = ConfigurationFactory::get()->findEntry(Database::databaseLocation);

                //create base directory
                if (entry)
                {
                    boost::filesystem::path storage(entry->value());

                    storage /= "MySQL";

                    if (boost::filesystem::exists(storage) == false)
                        status = boost::filesystem::create_directories(storage);

                    if (status)
                    {
                        //start mysql process
                        const std::string storageString = storage.string();
                        QString storagePath(storageString.c_str());
                        storagePath += "/";

                        const QString socketPath = m_server.run_server(storagePath);

                        if (socketPath.isEmpty() == false)
                        {
                            QSqlDatabase db_obj;
                            //setup db connection
                            db_obj = QSqlDatabase::addDatabase("QMYSQL", QDatabaseName);
                            db_obj.setConnectOptions("UNIX_SOCKET=" + socketPath);
                            db_obj.setHostName("localhost");
                            db_obj.setUserName("root");

                            *db = db_obj;
                        }

                        m_initialized = socketPath.isEmpty() == false;;
                    }
                }
            }

            return status;
        }

        bool m_initialized;
        MySqlServer m_server;
    };


    MySqlBackend::MySqlBackend(): m_data(new Data)
    {

    }


    MySqlBackend::~MySqlBackend()
    {
        //TODO: assert for db closed. In needs to be closed before MySQL server termination
    }


    bool MySqlBackend::prepareDB(QSqlDatabase *db)
    {
        return m_data->prepareDB(db);
    }


    QString MySqlBackend::prepareCreationQuery(const QString& name, const QString& columns) const
    {
        //Here we force InnoDB engine which may be default
        //http://dev.mysql.com/doc/refman/5.5/en/innodb-default-se.html

        const QString result = QString("CREATE TABLE %1(%2) ENGINE=InnoDB;").arg(name).arg(columns);

        return result;
    }
}
