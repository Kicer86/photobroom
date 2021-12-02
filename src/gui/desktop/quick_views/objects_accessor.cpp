
#include "objects_accessor.hpp"


ObjectsAccessor::ObjectsAccessor(QObject* parent)
    : QObject(parent)
    , m_database(nullptr)
    , m_core(nullptr)
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


void ObjectsAccessor::setCoreFactory(ICoreFactoryAccessor* core)
{
    m_core = core;
}


Database::IDatabase* ObjectsAccessor::database() const
{
    return m_database;
}

ICoreFactoryAccessor* ObjectsAccessor::coreFactory() const
{
    return m_core;
}
