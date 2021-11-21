
#include "observables_registry.hpp"


ObservablesRegistry& ObservablesRegistry::instance()
{
    static ObservablesRegistry registry;

    return registry;
}


void ObservablesRegistry::enable(bool en)
{
    m_enabled = en;

    emit enabledChanged(m_enabled);

    if (m_enabled == false)
    {
        m_executors.clear();

        emit executorsChanged(m_executors);
    }
}


bool ObservablesRegistry::isEnabled() const
{
    return m_enabled;
}


const QSet<ObservableExecutor *>& ObservablesRegistry::executors() const
{
    return m_executors;
}


void ObservablesRegistry::add(ObservableExecutor* executor)
{
    // postpone add, 'executor' could have called 'add()' from its constructor
    // and therefore may be not fully constructed which may cause some pure virtual calls
    if (m_enabled)
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
