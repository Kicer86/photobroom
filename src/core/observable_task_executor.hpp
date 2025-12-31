
#ifndef OBSERVABLE_TASK_EXECUTOR_HPP_INCLUDED
#define OBSERVABLE_TASK_EXECUTOR_HPP_INCLUDED

#include <boost/type_index.hpp>

#include <core/itask_executor.hpp>
#include <core/observable_executor.hpp>


template<typename T> requires std::is_base_of<ITaskExecutor, T>::value
class ObservableTaskExecutor: public ObservableExecutor, public T
{
    public:
        template<typename ...Args>
        explicit ObservableTaskExecutor(Args&&... args): T(std::forward<Args>(args)...) {}

        void add(std::unique_ptr<ITaskExecutor::ITask>&& task) override
        {
            std::unique_ptr<Task> observedTask = std::make_unique<Task>(*this, std::move(task));

            T::add(std::move(observedTask));
        }

        QString name() const override
        {
            return boost::typeindex::type_id<T>().pretty_name().c_str();
        }

    private:
        class Task final: public ITaskExecutor::ITask
        {
            public:
                Task(ObservableTaskExecutor& executor, std::unique_ptr<ITaskExecutor::ITask>&& task)
                    : m_task(std::move(task))
                    , m_executor(executor)
                {
                    m_executor.newTaskInQueue(name());
                }

                std::string name() const override
                {
                    return m_task->name();
                }

                void perform() override
                {
                    m_executor.taskMovedToExecution(name());
                    m_task->perform();
                    m_executor.taskExecuted(name());
                }

            private:
                std::unique_ptr<ITaskExecutor::ITask> m_task;
                ObservableTaskExecutor& m_executor;
        };
};


#endif
