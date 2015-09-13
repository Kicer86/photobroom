
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
                db_obj.setDatabaseName(prjInfo.projectDir + QDir::separator() + prjInfo.databaseLocation );
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
            status = Database::ASqlBackend::dbOpened();

        return status;
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


    void SQLiteBackend::set(IConfiguration *)
    {

    }


    SqlQuery SQLiteBackend::insertOrUpdate(const InsertQueryData& data) const
    {
        QString result("INSERT OR REPLACE INTO %1(%2) VALUES(%3)");

        result = result.arg(data.getName());
        result = result.arg(data.getColumns().join(", "));
        result = result.arg(data.getValues().join(", "));

        return result;
    }


    SQLitePlugin::SQLitePlugin(): IPlugin()
    {

    }


    SQLitePlugin::~SQLitePlugin()
    {

    }


    std::unique_ptr<IBackend> SQLitePlugin::constructBackend()
    {
        return std::unique_ptr<IBackend>(new SQLiteBackend);
    }


    QString SQLitePlugin::backendName() const
    {
        return "SQLite";
    }


    ProjectInfo SQLitePlugin::initPrjDir(const QString&) const
    {
        ProjectInfo prjInfo;
        prjInfo.backendName = backendName();
        prjInfo.databaseLocation = "broom.db";

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
