
#ifndef JOBS_MANAGER_HPP
#define JOBS_MANAGER_HPP

#include <future>

#include "task_executor_utils.hpp"


template<typename E, typename T>
class JobsManager
{
    public:
        JobsManager(E* executor, T&& task):
            m_executor(executor),
            m_task(std::move(task))
        {

        }

        template<typename R>
        auto wait()
        {
            typedef std::packaged_task<R> PTask;

            PTask p_task(std::move(m_task));

            typedef std::packaged_task<R> PTask;

            auto result_future = p_task.get_future();
            ExecutorTraits<E, PTask>::exec(m_executor, std::move(p_task));

            result_future.wait();

            const auto result = result_future.get();

            return result;
        }

        template<typename C>
        void callback(C&& callback_function)
        {
            auto p_task = [task = std::move(m_task), cf = std::move(callback_function)]()
            {
                cf(task());
            };

            ExecutorTraits<E, decltype(p_task)>::exec(m_executor, std::move(p_task));
        }

    private:
        E* m_executor;
        T m_task;
};


template<typename E, typename T>
JobsManager<E, T> job(E* executor, T&& task)
{
    return JobsManager(executor, std::move(task));
}

#endif
