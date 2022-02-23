
#include "objects_accessor.hpp"


ObjectsAccessor::ObjectsAccessor(QObject* parent)
    : QObject(parent)
    , m_database(nullptr)
    , m_project(nullptr)
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


void ObjectsAccessor::setProject(Project* prj)
{
    m_project = prj;

    emit projectChanged(prj);
}


void ObjectsAccessor::setCoreFactory(ICoreFactoryAccessor* core)
{
    m_core = core;

    emit coreFactoryChanged(core);
}


Database::IDatabase* ObjectsAccessor::database() const
{
    return m_database;
}


Project* ObjectsAccessor::project() const
{
    return m_project;
}


ICoreFactoryAccessor* ObjectsAccessor::coreFactory() const
{
    return m_core;
}
