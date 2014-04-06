
#include "backend.hpp"

#include <stdexcept>

#include <QSqlDatabase>

#include <boost/filesystem.hpp>

#include <configuration/configurationfactory.hpp>
#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>

#include "databasebuilder.hpp"


namespace Database
{
    const char* QDatabaseName = "Backend";

    struct SQLiteBackend::Data
    {
        Data(): m_initialized(false) {}

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

                    storage /= "SQLite";

                    if (boost::filesystem::exists(storage) == false)
                        status = boost::filesystem::create_directories(storage);

                    if (status)
                    {

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
        //TODO: assert for db closed. In needs to be closed before MySQL server termination
    }


    bool SQLiteBackend::prepareDB(QSqlDatabase *db)
    {
        return m_data->prepareDB(db);
    }


    QString SQLiteBackend::prepareCreationQuery(const QString& name, const QString& columns) const
    {
        //Here we force InnoDB engine which may be default
        //http://dev.mysql.com/doc/refman/5.5/en/innodb-default-se.html

        const QString result = QString("CREATE TABLE %1(%2) ENGINE=InnoDB;").arg(name).arg(columns);

        return result;
    }
}
