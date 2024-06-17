
#include <QDir>

#include "plugin.hpp"
#include "backend.hpp"


namespace Database
{

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

