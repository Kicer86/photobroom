
#ifndef TASK_EXECUTOR_UTILS
#define TASK_EXECUTOR_UTILS

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


// Helper class.
// A subqueue for ITaskExecutor.
// Its purpose is to have a queue of tasks to be executed by executor
// but controled by client ( can be clean()ed )
class CORE_EXPORT TasksQueue
{
    public:

        TasksQueue(ITaskExecutor *);

        void push(std::unique_ptr<ITaskExecutor::ITask>&& callable)
        {
            std::lock_guard<std::mutex> guard(m_tasksMutex);

            auto task = std::make_unique<IntTask>(std::move(callable), this);
            m_waitingTasks.push_back(std::move(task));

            try_to_fire();
        }

        void clear();

    private:
        friend struct IntTask;

        struct IntTask: ITaskExecutor::ITask
        {
            IntTask(std::unique_ptr<ITaskExecutor::ITask>&& callable, TasksQueue* queue):
                m_callable(std::move(callable)),
                m_queue(queue)
            {
            }

            void notify()
            {
                // tell TasksQueue job is done
                m_queue->task_finished();
            }

            void perform() override
            {
                m_callable->perform();     // client's code
                notify();                  // internal jobs
            }

            std::string name() const override
            {
                return "TasksQueue::IntTask";
            }

            std::unique_ptr<ITaskExecutor::ITask> m_callable;
            TasksQueue* m_queue;
        };

        std::mutex m_tasksMutex;
        std::deque<std::unique_ptr<ITaskExecutor::ITask>> m_waitingTasks;
        ITaskExecutor* m_tasksExecutor;
        int m_maxTasks;
        int m_executingTasks;

        void try_to_fire();
        void fire();
        void task_finished();
};

#endif
