
#ifndef OBSERVABLE_EXECUTOR_HPP_INCLUDED
#define OBSERVABLE_EXECUTOR_HPP_INCLUDED

#include <QTimer>

#include <core_export.h>


class CORE_EXPORT ObservableExecutor: public QObject
{
    Q_OBJECT

    public:
        ObservableExecutor();
        virtual ~ObservableExecutor();

        Q_PROPERTY(int awaitingTasks READ awaitingTasks NOTIFY awaitingTasksChanged)
        Q_PROPERTY(int tasksExecuted READ tasksExecuted NOTIFY tasksExecutedChanged)
        Q_PROPERTY(double executionSpeed READ executionSpeed NOTIFY executionSpeedChanged)
        Q_PROPERTY(QString name READ name NOTIFY nameChanged)

        int awaitingTasks() const;
        int tasksExecuted() const;
        virtual QString name() const = 0;
        double executionSpeed() const;

    signals:
        void awaitingTasksChanged(int) const;
        void tasksExecutedChanged(int) const;
        void nameChanged(QString) const;
        void executionSpeedChanged(double) const;

    protected:
        void newTaskInQueue();
        void taskMovedToExecution();
        void taskExecuted();

    private:
        QTimer m_executionSpeedTimer;
        std::atomic<int> m_awaitingTasks = 0;
        std::atomic<int> m_tasksExecuted = 0;

        std::atomic<int> m_executionSpeedCounter = 0;
        std::array<int, 5> m_executionSpeedBuffer;
        std::size_t m_executionSpeedBufferPos = 0;
        double m_executionSpeed = 0.0;

        void updateExecutionSpeed();
};

#endif
