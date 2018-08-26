
#ifndef TASK_EXECUTOR_UTILS
#define TASK_EXECUTOR_UTILS

#include <mutex>
#include <future>

#include "itask_executor.hpp"

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
class TasksQueue
{
    public:

        TasksQueue(ITaskExecutor *);

        template<typename T>
        void addTask(T&& callable)
        {
            std::lock_guard<std::mutex> guard(m_tasksMutex);

            auto task = std::make_unique<IntTask<T>>(callable, this);
            m_waitingTasks.push_back(task);

            try_to_fire();
        }

        void clean();

    private:
        template<typename T> friend struct IntTask;

        template<typename T>
        struct IntTask: ITaskExecutor::ITask
        {
            IntTask(T&& callable, TasksQueue* queue):
                m_callable(callable),
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
                m_callable();     // client's code
                notify();         // internal jobs
            }

            std::string name() override
            {
                return "TasksQueue::IntTask";
            }

            T m_callable;
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
