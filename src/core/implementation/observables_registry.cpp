
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
    // postpone add, 'executor' can be call 'add()' from its constructor
    // and therefore may be not fully constructed which may cause some pure virtual calls
    QMetaObject::invokeMethod(this, [executor, this]()
    {
        m_executors.insert(executor);

        emit executorsChanged(m_executors);
    },
    Qt::QueuedConnection);
}


void ObservablesRegistry::remove(ObservableExecutor* executor)
{
    m_executors.remove(executor);

    emit executorsChanged(m_executors);
}
