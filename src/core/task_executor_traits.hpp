
#ifndef TASK_EXECUTOR_TRAITS_HPP_INCLUDED
#define TASK_EXECUTOR_TRAITS_HPP_INCLUDED


#include <core/task_executor_utils.hpp>

template<typename T>
struct ExecutorTraits<ITaskExecutor, T>
{
    static void exec(ITaskExecutor& executor, T&& t, const std::string& name = std::source_location::current().function_name())
    {
        struct Task: ITaskExecutor::ITask
        {
            Task(const std::string& name, T&& t): m_name(name), m_task(std::move(t)) {}

            std::string name() const override { return m_name; }
            void perform() override { m_task(); }

            const std::string m_name;
            T m_task;
        };

        executor.add(std::make_unique<Task>(name, std::move(t)));
    }
};


#endif // TASK_EXECUTOR_TRAITS_HPP_INCLUDED
