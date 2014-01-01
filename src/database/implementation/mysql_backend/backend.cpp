
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

    struct MySqlBackend::Data
    {
        Data(): m_initialized(false), m_server() {}

        bool openDB(QSqlDatabase* db) noexcept(true)
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
                        const QString storagePath(storage.c_str());
                        const QString socketPath = m_server.run_server(storagePath);

                        if (socketPath.isEmpty() == false)
                        {
                            //setup db connection
                            *db = QSqlDatabase::addDatabase("QMYSQL", "Backend");
                            db->setConnectOptions("UNIX_SOCKET=" + socketPath);
                        }

                        m_initialized = socketPath.isEmpty() == false;;
                    }
                }
            }

            return status;
        }

        private:
            bool m_initialized;
            MySqlServer m_server;
    };


    MySqlBackend::MySqlBackend(): m_data(new Data)
    {

    }


    MySqlBackend::~MySqlBackend()
    {
    }


    bool MySqlBackend::openDB(QSqlDatabase *db)
    {
        return m_data->openDB(db);
    }
}
