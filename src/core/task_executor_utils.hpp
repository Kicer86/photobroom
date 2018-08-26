
#ifndef TASK_EXECUTOR_UTILS
#define TASK_EXECUTOR_UTILS

#include <future>

template<typename T, typename E>
struct ExecutorTraits
{
    static void exec(E *, T &&);
};


// Helper function.
// Run a task and wait for it to be finished.
template<typename R, typename E, typename T>
auto evaluate(E* executor, const T& task)
{
    typedef std::packaged_task<R> PTask;

    PTask p_task(task);

    auto result_future = p_task.get_future();
    ExecutorTraits<E, PTask>::exec(executor, std::move(p_task));

    result_future.wait();

    const auto result = result_future.get();

    return result;
}

#endif
