
#include "objects_accessor.hpp"


ObjectsAccessor::ObjectsAccessor(QObject* parent)
    : QObject(parent)
    , m_project(nullptr)
    , m_core(nullptr)
{

}


ObjectsAccessor& ObjectsAccessor::instance()
{
    static ObjectsAccessor i;

    return i;
}


void ObjectsAccessor::setProject(Project* prj)
{
    m_project = prj;

    emit projectChanged(prj);
    emit projectOpenChanged(projectOpen());
    emit databaseChanged(database());
}


void ObjectsAccessor::setCoreFactory(ICoreFactoryAccessor* core)
{
    m_core = core;

    emit coreFactoryChanged(core);
}


void ObjectsAccessor::setRecentProjects(const QStringList& recent)
{
    m_recentProjects = recent;
    emit recentProjectsChanged(recent);
}


Database::IDatabase* ObjectsAccessor::database() const
{
    return m_project == nullptr? nullptr: &m_project->getDatabase();
}


Project* ObjectsAccessor::project() const
{
    return m_project;
}


ICoreFactoryAccessor* ObjectsAccessor::coreFactory() const
{
    return m_core;
}


const QStringList& ObjectsAccessor::recentProjects() const
{
    return m_recentProjects;
}


bool ObjectsAccessor::projectOpen() const
{
    return m_project != nullptr;
}
