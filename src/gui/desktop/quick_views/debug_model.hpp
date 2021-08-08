
#ifndef DEBUGMODEL_HPP
#define DEBUGMODEL_HPP

#include <QObject>


class DebugModel: public QObject
{
        Q_OBJECT

    public:
        Q_PROPERTY(int dbTasks READ dbTasks NOTIFY dbTasksChanged)
        Q_PROPERTY(int executorTasks READ executorTasks NOTIFY executorTasksChanged)

        int dbTasks() const;
        int executorTasks() const;

    signals:
        void dbTasksChanged(int);
        void executorTasksChanged(int);

    private:
        int m_dbTasks;
        int m_executorTasks;
};

#endif
