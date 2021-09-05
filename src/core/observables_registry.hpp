
#ifndef OBSERVABLES_REGISTRY_HPP_INCLUDED
#define OBSERVABLES_REGISTRY_HPP_INCLUDED


#include <QObject>
#include <QSet>

#include <core/observable_executor.hpp>
#include <core_export.h>


class CORE_EXPORT ObservablesRegistry: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSet<ObservableExecutor *> executors READ executors NOTIFY executorsChanged)

    public:
        static ObservablesRegistry& instance();

        const QSet<ObservableExecutor *>& executors() const;
        void add(ObservableExecutor *);
        void remove(ObservableExecutor *);

    signals:
        void executorsChanged(const QSet<ObservableExecutor *> &) const;

    private:
        QSet<ObservableExecutor *> m_executors;

        ObservablesRegistry() = default;
};

#endif
