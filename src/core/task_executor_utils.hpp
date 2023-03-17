
#ifndef TASK_EXECUTOR_UTILS
#define TASK_EXECUTOR_UTILS

#include <deque>
#include <mutex>
#include <future>
#include <condition_variable>
#include <source_location>
#include <QFuture>
#include <QPromise>

#include "itask_executor.hpp"

#include "core_export.h"


template<typename T, typename E>
struct ExecutorTraits
{
    static void exec(E &, T &&);
};


class abort_exception: public std::exception {};


class CORE_EXPORT WorkState
{
    public:
        WorkState() = default;
        WorkState(const WorkState &) = delete;
        WorkState& operator=(const WorkState &) = delete;

        /**
         * @brief Set state to abort
         */
        void abort();

        /**
         * @brief Throw exception if state was set to abort
         * @throw abort_exception
         */
        void throwIfAbort();

    private:
        bool m_abort = false;
};


// Helper function.
// Run a task and wait for it to be finished.
template<typename R, typename E, typename T>
[[nodiscard]]
auto evaluate(E& executor, const T& task)
{
    typedef std::packaged_task<R> PTask;

    PTask p_task(task);

    auto result_future = p_task.get_future();
    ExecutorTraits<E, PTask>::exec(executor, std::move(p_task));

    result_future.wait();

    return result_future.get();
}


// Helper function.
// Run a task and do not wait for it to be finished.
template<typename E, typename T>
void execute(E& executor, T&& task)
{
    ExecutorTraits<E, T>::exec(executor, std::move(task));
}


// Run callable as a task
template<typename Callable>
void runOn(ITaskExecutor& executor, Callable&& callable, const std::string& taskName = std::source_location::current().function_name())
    requires std::is_invocable_v<Callable>
{
    struct GenericTask: ITaskExecutor::ITask
    {
        GenericTask(const std::string& name, Callable&& callable)
            : m_callable(std::forward<Callable>(callable))
            , m_name(name)
        {

        }

        std::string name() const override
        {
            return m_name;
        }

        void perform() override
        {
            m_callable();
        }

        private:
            typename std::remove_reference<Callable>::type m_callable;
            std::string m_name;
    };

    auto task = std::make_unique<GenericTask>(taskName, std::forward<Callable>(callable));
    executor.add(std::move(task));
}


// Run callable as a task
template<typename R, typename Callable>
QFuture<R> runOn(ITaskExecutor& executor, Callable&& callable, const std::string& taskName = std::source_location::current().function_name())
{
    struct GenericTask: ITaskExecutor::ITask
    {
        GenericTask(const std::string& name, Callable&& callable, QPromise<R>&& p)
            : m_callable(std::forward<Callable>(callable))
            , m_name(name)
            , m_promise(std::move(p))
        {

        }

        std::string name() const override
        {
            return m_name;
        }

        void perform() override
        {
            m_promise.start();
            m_callable(m_promise);
            m_promise.finish();
        }

        private:
            typename std::remove_reference<Callable>::type m_callable;
            std::string m_name;
            QPromise<R> m_promise;
    };

    QPromise<R> promise;
    auto future = promise.future();

    auto task = std::make_unique<GenericTask>(taskName, std::forward<Callable>(callable), std::move(promise));
    executor.add(std::move(task));

    return future;
}


/**
 * @brief Base class for all kind of executor queues
 *
 * Purpose of this class is to have a base queue of tasks to be executed by any executor
 * but controled by client (can be cleaned)
 *
 * @see clear
 */
template<typename Task>
class Queue
{
    public:
        /// Determines queue behavior
        enum class Mode
        {
            Fifo,               ///< Execute tasks in order of insertion. First inserted will be first to execute.
            Lifo,               ///< Execute tasks in reversed order of insertion. Last inserted will be first to execute.
        };

        Queue(int maxTasks, Mode mode = Mode::Fifo)
            : m_maxTasks(maxTasks)
            , m_mode(mode)
        {

        }

        ~Queue()
        {
            clear();  // drop all tasks awaiting

            waitForPendingTasks();
        }

        /**
         * @brief Insert new task to queue.
         */
        void push(Task&& task)
        {
            std::lock_guard<std::recursive_mutex> guard(m_tasksMutex);

            m_waitingTasks.push_back(std::move(task));

            try_to_fire();
        }

        /**
         * @brief Remove queued items.
         *
         * Remove tasks staying in queue. Tasks already being executed are not touched.
         */
        void clear()
        {
            std::lock_guard<std::recursive_mutex> guard(m_tasksMutex);
            m_waitingTasks.clear();
        }

        /**
         * @returns number of items wating in queue. Tasks being executed are not counted in.
         */
        std::size_t size() const
        {
            return m_waitingTasks.size();
        }

        /**
         * @brief Wait for tasks being executed.
         *
         * Blocks current thread until all tasks being executed are done.
         */
        void waitForPendingTasks()
        {
            // wait for tasks being executed
            std::unique_lock<std::recursive_mutex> lock(m_tasksMutex);
            m_noWork.wait(lock, [this]
            {
                return m_executingTasks == 0;
            });
        }

    protected:
        typedef std::shared_ptr<void> Notifier;
        virtual void passTaskToExecutor(Task&& task, const Notifier &) = 0;

    private:
        std::recursive_mutex m_tasksMutex;
        std::deque<Task> m_waitingTasks;
        std::condition_variable_any m_noWork;
        int m_maxTasks = 0;
        int m_executingTasks = 0;
        const Mode m_mode = Mode::Fifo;

        void task_finished_notification(void *)
        {
            task_finished();
        }

        void try_to_fire()
        {
            std::lock_guard<std::recursive_mutex> guard(m_tasksMutex);

            // Do not put all waiting tasks to executor.
            while (m_maxTasks > m_executingTasks && m_waitingTasks.empty() == false)
            {
                fire();
            }
        }

        void fire()
        {
            std::lock_guard<std::recursive_mutex> guard(m_tasksMutex);
            assert(m_waitingTasks.empty() == false);

            auto task = m_mode == Mode::Fifo? take_front(m_waitingTasks): take_back(m_waitingTasks);

            m_executingTasks++;
            auto notifier = Notifier(nullptr, std::bind(&Queue::task_finished_notification, this, std::placeholders::_1));
            passTaskToExecutor(std::move(task), notifier);
        }

        void task_finished()
        {
            // It is possible that in this function TasksQueue::fire() will be on stack
            // of current thread (if addTask(std::move(task), notifier); executes task immediately)
            // That's why we need recursive mutex here
            std::lock_guard<std::recursive_mutex> guard(m_tasksMutex);

            assert(m_executingTasks > 0);
            m_executingTasks--;

            if (m_executingTasks == 0)
                m_noWork.notify_one();

            try_to_fire();
        }
};


/**
 * @brief A subqueue for ITaskExecutor.
 */
class CORE_EXPORT TasksQueue: public ITaskExecutor, public Queue<std::unique_ptr<ITaskExecutor::ITask>>
{
    public:
        explicit TasksQueue(ITaskExecutor &, Mode = Mode::Fifo);
        ~TasksQueue() = default;

        // ITaskExecutor overrides:
        void add(std::unique_ptr<ITask> &&) override;
        IProcessControl* add(Process &&) override;
        int heavyWorkers() const override;

    private:
        friend struct IntTask;
        struct IntTask;

        ITaskExecutor& m_executor;

        void passTaskToExecutor(std::unique_ptr<ITask> &&, const Notifier &) override;
};


CORE_EXPORT std::unique_ptr<ITaskExecutor::ITask> inlineTask(const std::string& name, std::function<void()>&& task);

#endif
