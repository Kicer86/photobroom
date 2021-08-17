
#ifndef OBSERVABLE_EXECUTOR_HPP_INCLUDED
#define OBSERVABLE_EXECUTOR_HPP_INCLUDED

#include <QObject>

#include <core_export.h>


class CORE_EXPORT ObservableExecutor: public QObject
{
    Q_OBJECT

    public:
        Q_PROPERTY(int awaitingTasks READ awaitingTasks NOTIFY awaitingTasksChanged)
        Q_PROPERTY(int tasksExecuted READ tasksExecuted NOTIFY tasksExecutedChanged)

        int awaitingTasks() const
        {
            return m_awaitingTasks;
        }

        int tasksExecuted() const
        {
            return m_tasksExecuted;
        }

    signals:
        void awaitingTasksChanged(int) const;
        void tasksExecutedChanged(int) const;

    protected:
        void newTaskInQueue()
        {
            m_awaitingTasks++;

            emit awaitingTasksChanged(m_awaitingTasks);
        }

        void taskMovedToExecution()
        {
            m_awaitingTasks--;
            m_tasksExecuted++;

            emit awaitingTasksChanged(m_awaitingTasks);
            emit tasksExecutedChanged(m_tasksExecuted);
        }

        void taskExecuted()
        {
            m_tasksExecuted--;

            emit tasksExecutedChanged(m_tasksExecuted);
        }

    private:
        std::atomic<int> m_awaitingTasks = 0;
        std::atomic<int> m_tasksExecuted = 0;
};

#endif
