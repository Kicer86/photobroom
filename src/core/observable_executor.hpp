
#ifndef OBSERVABLE_EXECUTOR_HPP_INCLUDED
#define OBSERVABLE_EXECUTOR_HPP_INCLUDED

#include <QObject>
#include <mutex>

#include <core/itask_executor.hpp>


class ObservableExecutorProperties: public QObject
{
    Q_OBJECT

    public:

    protected:
        int m_awaitingTasks = 0;
        int m_tasksExecuted = 0;
        std::mutex m_countersMutex;

        void newTaskInQueue()
        {
            std::lock_guard<std::mutex> l(m_countersMutex);
            m_awaitingTasks++;
        }

        void taskMovedToExecution()
        {
            std::lock_guard<std::mutex> l(m_countersMutex);
            m_awaitingTasks--;
            m_tasksExecuted++;
        }

        void taskExecuted()
        {
            std::lock_guard<std::mutex> l(m_countersMutex);
            m_tasksExecuted--;
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
