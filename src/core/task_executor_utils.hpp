
#ifndef TASK_EXECUTOR_UTILS
#define TASK_EXECUTOR_UTILS

#include <deque>
#include <mutex>
#include <future>

#include "itask_executor.hpp"

#include "core_export.h"


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


// Run callable as a task
template<typename Callable>
void runOn(ITaskExecutor* executor, Callable&& callable)
{
    struct GenericTask: ITaskExecutor::ITask
    {
        GenericTask(Callable&& callable):
            m_callable(std::forward<Callable>(callable))
        {

        }

        std::string name() const override
        {
            return "generic";
        }

        void perform() override
        {
            m_callable();
        }

        private:
            typename std::remove_reference<Callable>::type m_callable;
    };

    auto task = std::make_unique<GenericTask>(std::forward<Callable>(callable));
    executor->add(std::move(task));
}


// Helper class.
// A subqueue for ITaskExecutor.
// Its purpose is to have a queue of tasks to be executed by executor
// but controled by client ( can be clean()ed )
class CORE_EXPORT TasksQueue final: public ITaskExecutor
{
    public:
        TasksQueue(ITaskExecutor *);
        ~TasksQueue();

        void push(std::unique_ptr<ITaskExecutor::ITask> &&);
        void clear();

        void add(std::unique_ptr<ITask> && ) override;
        void addLight(std::unique_ptr<ITask> && ) override;
        int heavyWorkers() const override;

    private:
        friend struct IntTask;
        struct IntTask;

        std::mutex m_tasksMutex;
        std::deque<std::unique_ptr<ITaskExecutor::ITask>> m_waitingTasks;
        std::condition_variable m_noWork;
        ITaskExecutor* m_tasksExecutor;
        int m_maxTasks;
        int m_executingTasks;

        void try_to_fire();
        void fire();
        void task_finished();
};

#endif
