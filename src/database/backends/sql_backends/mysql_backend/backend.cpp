
#include "backend.hpp"

#include <stdexcept>

#include <QProcess>
#include <QSqlDatabase>
#include <QDir>

#include <core/iconfiguration.hpp>
#include <database/database_builder.hpp>
#include <database/project_info.hpp>
#include <backends/sql_backends/table_definition.hpp>
#include <database/backends/sql_backends/query_structs.hpp>

#include "mysql_server.hpp"


namespace Database
{

    struct MySqlBackend::Data
    {
        Data(): m_server(), m_dbLocation(), m_initialized(false) {}
        Data(const Data &) = delete;
        Data& operator=(const Data &) = delete;

        void set(IConfiguration* configuration)
        {
            m_server.set(configuration);
        }

        ~Data() { }

        MySqlServer m_server;
        QString m_dbLocation;
        bool m_initialized;
    };


    MySqlBackend::MySqlBackend(): m_data(new Data)
    {

    }


    MySqlBackend::~MySqlBackend()
    {
        //TODO: assert for db closed. In needs to be closed before MySQL server termination
    }


    BackendStatus MySqlBackend::prepareDB(const ProjectInfo& prjInfo)
    {
        BackendStatus status = StatusCodes::Ok;

        if (m_data->m_initialized == false)
        {
            m_data->m_dbLocation = prjInfo.databaseLocation;

            //start mysql process
            const QString socketPath = m_data->m_server.run_server(m_data->m_dbLocation);

            if (socketPath.isEmpty() == false)
            {
                QSqlDatabase db_obj;
                //setup db connection
                db_obj = QSqlDatabase::addDatabase("QMYSQL", getConnectionName());
                db_obj.setConnectOptions("UNIX_SOCKET=" + socketPath);
                db_obj.setDatabaseName("photo_broom");
                db_obj.setHostName("localhost");
                db_obj.setUserName("root");
            }

            m_data->m_initialized = socketPath.isEmpty() == false;;
        }

        return status;
    }


    QString MySqlBackend::prepareCreationQuery(const QString& name, const QString& columns) const
    {
        //Here we force InnoDB engine which may be default
        //http://dev.mysql.com/doc/refman/5.5/en/innodb-default-se.html

        const QString result = QString("CREATE TABLE %1(%2) ENGINE=InnoDB;").arg(name).arg(columns);

        return result;
    }


    const IGenericSqlQueryGenerator* MySqlBackend::getGenericQueryGenerator() const
    {
        return this;
    }


    void MySqlBackend::set(IConfiguration* configuration)
    {
        m_data->set(configuration);
        m_data->m_server.set(configuration);
    }


    void MySqlBackend::set(ILoggerFactory* logger)
    {
        Database::ASqlBackend::set(logger);

        m_data->m_server.set(logger);
    }


    QString MySqlBackend::getTypeFor(ColDefinition::Purpose type) const
    {
        QString result;

        switch(type)
        {
            case ColDefinition::Purpose::ID:
                result = "INT PRIMARY KEY AUTO_INCREMENT";
                break;

            default:
                break;
        }

        return result;
    }


    MySqlPlugin::MySqlPlugin(): IPlugin()
    {

    }


    MySqlPlugin::~MySqlPlugin()
    {

    }


    std::unique_ptr<IBackend> MySqlPlugin::constructBackend()
    {
        return std::make_unique<MySqlBackend>();
    }


    QString MySqlPlugin::backendName() const
    {
        return "MySql";
    }


    ProjectInfo MySqlPlugin::initPrjDir(const QString& prjPath, const QString& name) const
    {
        const QString subdir = name + "_database_files";

        QDir prjPathDir(prjPath);
        prjPathDir.mkdir(subdir);

        ProjectInfo prjInfo;
        prjInfo.backendName = backendName();
        prjInfo.databaseLocation = "./" + subdir + "/";

        return prjInfo;
    }


    QLayout* MySqlPlugin::buildDBOptions()
    {
        return nullptr;
    }


    char MySqlPlugin::simplicity() const
    {
        return 0;
    }
}
