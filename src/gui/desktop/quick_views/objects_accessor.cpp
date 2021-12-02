
#include "objects_accessor.hpp"


ObjectsAccessor::ObjectsAccessor(QObject* parent)
    : QObject(parent)
    , m_database(nullptr)
{

}


ObjectsAccessor& ObjectsAccessor::instance()
{
    static ObjectsAccessor i;

    return i;
}


void ObjectsAccessor::setDatabase(Database::IDatabase* database)
{
    m_database = database;

    emit databaseChanged(database);
}


Database::IDatabase* ObjectsAccessor::database() const
{
    return m_database;
}
