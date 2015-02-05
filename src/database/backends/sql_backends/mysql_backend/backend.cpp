
#include "backend.hpp"

#include <stdexcept>

#include <QProcess>
#include <QSqlDatabase>
#include <QDir>
#include <QSqlQuery>

#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>
#include <database/database_builder.hpp>
#include <database/project_info.hpp>
#include <backends/sql_backends/table_definition.hpp>

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


    bool MySqlBackend::prepareDB(const ProjectInfo& prjInfo)
    {
        bool status = true;

        if (m_data->m_initialized == false)
        {
            m_data->m_dbLocation = prjInfo.projectDir +"/" + prjInfo.databaseLocation;

            //start mysql process
            const QString socketPath = m_data->m_server.run_server(m_data->m_dbLocation);

            if (socketPath.isEmpty() == false)
            {
                QSqlDatabase db_obj;
                //setup db connection
                db_obj = QSqlDatabase::addDatabase("QMYSQL", getConnectionName());
                db_obj.setConnectOptions("UNIX_SOCKET=" + socketPath);
                db_obj.setHostName("localhost");
                db_obj.setUserName("root");
            }

            m_data->m_initialized = socketPath.isEmpty() == false;;
        }

        return status;
    }


    bool MySqlBackend::onAfterOpen()
    {
        const QString mysql_db("photo_broom");
        //check if database exists
        QSqlDatabase db = QSqlDatabase::database(getConnectionName());
        QSqlQuery query(db);
        bool status = exec(QString("SHOW DATABASES LIKE '%1'").arg(mysql_db), &query);

        //create database if doesn't exists
        bool empty = query.next() == false;

        if (status && empty)
            status = exec(QString("CREATE DATABASE %1;").arg(mysql_db), &query);

        //Reconnect to database. Not nice, but is there any other way?
        db.setDatabaseName(mysql_db);
        db.close();
        db.open();

        return status;
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
                result = col.name + " " + "INT PRIMARY KEY AUTO_INCREMENT";
                break;
        }

        return result;
    }


    const ISqlQueryConstructor* MySqlBackend::getQueryConstructor() const
    {
        return this;
    }


    void MySqlBackend::set(IConfiguration* configuration)
    {
        m_data->set(configuration);
        m_data->m_server.set(configuration);
    }


    void MySqlBackend::set(ILogger* logger)
    {
        Database::ASqlBackend::set(logger);

        m_data->m_server.set(logger);
    }



    SqlQuery MySqlBackend::insertOrUpdate(const InsertQueryData &) const
    {
        assert(!"not implemented");

        return SqlQuery();
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
    
    
    QString MySqlPlugin::backendName() const
    {
        return "MySql";
    }


    ProjectInfo MySqlPlugin::initPrjDir(const QString& prjPath) const
    {
        QDir prjPathDir(prjPath);
        prjPathDir.mkdir("database");

        ProjectInfo prjInfo;
        prjInfo.backendName = backendName();
        prjInfo.databaseLocation = "./database/";

        return prjInfo;
    }


    QLayout* MySqlPlugin::buildDBOptions()
    {
        return nullptr;
    }

}
