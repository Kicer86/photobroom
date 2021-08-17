
#ifndef OBSERVABLE_EXECUTOR_HPP_INCLUDED
#define OBSERVABLE_EXECUTOR_HPP_INCLUDED

#include <QObject>
#include <mutex>

#include <core/itask_executor.hpp>


class ObservableExecutorProperties: public QObject
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
        std::atomic<int> m_awaitingTasks = 0;
        std::atomic<int> m_tasksExecuted = 0;

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
};

template<typename T> requires std::is_base_of<ITaskExecutor, T>::value
class ObservableExecutor: public ObservableExecutorProperties, public T
{
    public:
        template<typename ...Args>
        ObservableExecutor(Args... args): T(args...) {}

        void add(std::unique_ptr<ITaskExecutor::ITask>&& task) override
        {
            std::unique_ptr<Task> observedTask = std::make_unique<Task>(std::move(task));

            T::add(std::move(observedTask));
        }

        void addLight(std::unique_ptr<ITaskExecutor::ITask>&& task) override
        {
            std::unique_ptr<Task> observedTask = std::make_unique<Task>(std::move(task));

            T::addLight(std::move(observedTask));
        }

    private:
        class Task: public ITaskExecutor::ITask
        {
            public:
                Task(std::unique_ptr<ITaskExecutor::ITask>&& task)
                    : m_task(std::move(task))
                {

                }

                std::string name() const override
                {
                    return m_task->name();
                }

                void perform() override
                {
                    m_task->perform();
                }

            private:
                std::unique_ptr<ITaskExecutor::ITask> m_task;
        };
};

#endif
