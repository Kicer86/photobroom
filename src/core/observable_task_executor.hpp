
#ifndef OBSERVABLE_TASK_EXECUTOR_HPP_INCLUDED
#define OBSERVABLE_TASK_EXECUTOR_HPP_INCLUDED

#include <core/itask_executor.hpp>
#include <core/observable_executor.hpp>


template<typename T> requires std::is_base_of<ITaskExecutor, T>::value
class ObservableTaskExecutor: public ObservableExecutor, public T
{
    public:
        template<typename ...Args>
        ObservableTaskExecutor(Args... args): T(args...) {}

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
