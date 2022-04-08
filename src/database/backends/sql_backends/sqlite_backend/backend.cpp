
#include "backend.hpp"

#include <stdexcept>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QDir>

#include <database/project_info.hpp>
#include <backends/sql_backends/table_definition.hpp>
#include <backends/sql_backends/query_structs.hpp>

namespace Database
{

    struct SQLiteBackend::Data
    {
        Data(): m_initialized(false) {}

        ~Data()
        {

        }

        BackendStatus prepareDB(ASqlBackend* backend, const ProjectInfo& prjInfo)
        {
            BackendStatus status = StatusCodes::Ok;

            if (m_initialized == false)
            {
                QSqlDatabase db_obj;
                //setup db connection
                db_obj = QSqlDatabase::addDatabase("QSQLITE", backend->getConnectionName());

                /// TODO: use some nice way for setting database name here
                db_obj.setDatabaseName(prjInfo.databaseLocation );
            }

            return status;
        }

        bool m_initialized;
    };


    SQLiteBackend::SQLiteBackend(IConfiguration *, ILogger* l): ASqlBackend(l), m_data(new Data)
    {

    }


    SQLiteBackend::~SQLiteBackend()
    {

    }


    BackendStatus SQLiteBackend::prepareDB(const ProjectInfo& prjInfo)
    {
        return m_data->prepareDB(this, prjInfo);
    }


    bool SQLiteBackend::dbOpened()
    {
        QSqlDatabase db = QSqlDatabase::database(getConnectionName());
        QSqlQuery query(db);

        bool status = query.exec("PRAGMA journal_mode = WAL;");

        if (status)
            status = query.exec("PRAGMA synchronous = NORMAL;"); // TODO: dangerous, use some backups?

        if (status)
            status = query.exec("PRAGMA foreign_keys = ON;");

        if (status)
            status = Database::ASqlBackend::dbOpened();

        return status;
    }


    QString SQLiteBackend::prepareFindTableQuery(const QString& name) const
    {
        return QString("SELECT name FROM sqlite_master WHERE name='%1';").arg(name);
    }


    const IGenericSqlQueryGenerator& SQLiteBackend::getGenericQueryGenerator() const
    {
        return *this;
    }


    QString SQLiteBackend::getTypeFor(ColDefinition::Purpose type) const
    {
        QString result;

        switch(type)
        {
            case ColDefinition::Purpose::ID:
                // Do not use AUTOINCREMENT keyword:
                // http://www.sqlite.org/autoinc.html
                // http://stackoverflow.com/questions/10727541/running-out-of-unique-ids-in-sqlite-database
                result = "INTEGER PRIMARY KEY";
                break;

            default:
                break;
        }

        return result;
    }


    SQLitePlugin::SQLitePlugin(): IPlugin()
    {

    }


    SQLitePlugin::~SQLitePlugin()
    {

    }


    std::unique_ptr<IBackend> SQLitePlugin::constructBackend(IConfiguration* c, ILogger* l)
    {
        return std::make_unique<SQLiteBackend>(c, l);
    }


    QString SQLitePlugin::backendName() const
    {
        return "SQLite";
    }


    ProjectInfo SQLitePlugin::initPrjDir(const QString& dir, const QString &) const
    {
        ProjectInfo prjInfo;
        prjInfo.backendName = backendName();
        prjInfo.databaseLocation = dir + QDir::separator() + "photo_broom.db";

        return prjInfo;
    }


    QLayout* SQLitePlugin::buildDBOptions()
    {
        return nullptr;
    }


    char SQLitePlugin::simplicity() const
    {
        return 127;
    }

}
