
#include "observables_registry.hpp"


ObservablesRegistry& ObservablesRegistry::instance()
{
    static ObservablesRegistry registry;

    return registry;
}


const QSet<ObservableExecutor *>& ObservablesRegistry::executors() const
{
    return m_executors;
}


void ObservablesRegistry::add(ObservableExecutor* executor)
{
    m_executors.insert(executor);

    emit executorsChanged(m_executors);
}



void ObservablesRegistry::remove(ObservableExecutor* executor)
{
    m_executors.remove(executor);

    emit executorsChanged(m_executors);
}
