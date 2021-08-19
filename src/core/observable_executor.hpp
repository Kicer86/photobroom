
#ifndef OBSERVABLE_EXECUTOR_HPP_INCLUDED
#define OBSERVABLE_EXECUTOR_HPP_INCLUDED

#include <QObject>

#include <core_export.h>


class CORE_EXPORT ObservableExecutor: public QObject
{
    Q_OBJECT

    public:
        ObservableExecutor();
        virtual ~ObservableExecutor();

        Q_PROPERTY(int awaitingTasks READ awaitingTasks NOTIFY awaitingTasksChanged)
        Q_PROPERTY(int tasksExecuted READ tasksExecuted NOTIFY tasksExecutedChanged)
        Q_PROPERTY(QString name READ name NOTIFY nameChanged)

        int awaitingTasks() const;
        int tasksExecuted() const;
        virtual QString name() const = 0;

    signals:
        void awaitingTasksChanged(int) const;
        void tasksExecutedChanged(int) const;
        void nameChanged(QString) const;

    protected:
        void newTaskInQueue();
        void taskMovedToExecution();
        void taskExecuted();

    private:
        std::atomic<int> m_awaitingTasks = 0;
        std::atomic<int> m_tasksExecuted = 0;
};

#endif
