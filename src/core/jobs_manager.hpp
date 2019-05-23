
#ifndef JOBS_MANAGER_HPP
#define JOBS_MANAGER_HPP

#include <future>

#include "function_wrappers.hpp"
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

        template<typename...Args, typename C>
        void execute(C&& callback_function)
        {
            auto p_task = [task = std::move(m_task), cf = std::move(callback_function), executor = m_executor]()
            {
                cf(task(executor));
            };

            ExecutorTraits<E, decltype(p_task)>::exec(m_executor, std::move(p_task));
        }

        template<typename...Args, typename C>
        void execute2(QObject* o, C&& callback)
        {
            auto ctf = make_cross_thread_function<Args...>(o, std::move(callback));
            execute(ctf);
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
