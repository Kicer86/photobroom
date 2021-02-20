
#include "bridge.hpp"


Bridge::Bridge(QObject* parent)
    : QObject(parent)
    , m_database(nullptr)
{

}

void Bridge::setDatabase(Database::IDatabase* database)
{
    m_database = database;

    emit databaseChanged(database);
}


Database::IDatabase* Bridge::database() const
{
    return m_database;
}
