
#ifndef OBSERVABLE_EXECUTOR_HPP_INCLUDED
#define OBSERVABLE_EXECUTOR_HPP_INCLUDED

#include <array>
#include <atomic>
#include <string>
#include <vector>

#include <QHash>
#include <QObject>
#include <QString>
#include <QTimer>

#include <core_export.h>


class CORE_EXPORT ObservableExecutor: public QObject
{
    Q_OBJECT

    public:
        struct TaskEntry
        {
            QString name;
            int count = 0;
        };

        ObservableExecutor();
        virtual ~ObservableExecutor();

        virtual QString name() const = 0;

        int awaitingTasks() const;
        int tasksExecuted() const;
        double executionSpeed() const;

        const std::vector<TaskEntry>& tasks() const;

    signals:
        void awaitingTasksChanged(int awaitingTasks) const;
        void tasksExecutedChanged(int tasksExecuted) const;
        void executionSpeedChanged(double speed) const;
        void taskEntryChanged(const QString& name, int count) const;

    protected:
        void newTaskInQueue(const std::string& name);
        void taskMovedToExecution(const std::string& name);
        void taskExecuted(const std::string& name);

    private:
        std::atomic<int> m_awaitingTasks = 0;
        std::atomic<int> m_tasksExecuted = 0;

        QTimer m_executionSpeedTimer;
        std::atomic<int> m_executionSpeedCounter = 0;
        std::array<int, 5> m_executionSpeedBuffer;
        std::size_t m_executionSpeedBufferPos = 0;
        double m_executionSpeed = 0.0;

        std::vector<TaskEntry> m_taskEntries;
        QHash<QString, int> m_taskRowForText;

        void updateExecutionSpeed();
        void notifyAwaitingTasksChanged();
        void notifyTasksExecutedChanged();
        void adjustTask(const QString& name, int delta);
};


#endif
