
#include <cassert>

#include "task_executor_utils.hpp"
#include "containers_utils.hpp"


namespace
{
    class InlineTask: public ITaskExecutor::ITask
    {
        public:
            InlineTask(const std::string& name, std::function<void()>&& task)
                : m_name(name)
                , m_task(task)
            {

            }

            std::string name() const override
            {
                return m_name;
            }

            void perform() override
            {
                m_task();
            }

        private:
            const std::string m_name;
            std::function<void()> m_task;
    };
}


struct TasksQueue::IntTask: ITaskExecutor::ITask
{
    IntTask(std::unique_ptr<ITaskExecutor::ITask>&& callable, TasksQueue* queue):
        m_task(std::move(callable)),
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
        m_task->perform();         // client's code
        notify();                  // internal jobs
    }

    std::string name() const override
    {
        return std::string("TasksQueue::IntTask: ") + m_task->name();
    }

    std::unique_ptr<ITaskExecutor::ITask> m_task;
    TasksQueue* m_queue;
};



TasksQueue::TasksQueue(ITaskExecutor* executor, Mode mode):
    m_tasksMutex(),
    m_waitingTasks(),
    m_tasksExecutor(executor),
    m_maxTasks(executor->heavyWorkers() + 2),
    m_executingTasks(0),
    m_mode(mode)
{

}


TasksQueue::~TasksQueue()
{
    clear();  // drop all tasks awaiting

    // wait for tasks being executed
    std::unique_lock<std::recursive_mutex> lock(m_tasksMutex);
    m_noWork.wait(lock, [this]
    {
        return m_executingTasks == 0;
    });
}


void TasksQueue::push(std::unique_ptr<ITaskExecutor::ITask>&& callable)
{
    std::lock_guard<std::recursive_mutex> guard(m_tasksMutex);

    auto task = std::make_unique<IntTask>(std::move(callable), this);
    m_waitingTasks.push_back(std::move(task));

    try_to_fire();
}


void TasksQueue::clear()
{
    std::lock_guard<std::recursive_mutex> guard(m_tasksMutex);
    m_waitingTasks.clear();
}


void TasksQueue::add(std::unique_ptr<ITask>&& task)
{
    push(std::move(task));
}


void TasksQueue::addLight(std::unique_ptr<ITask>&& task)
{
    m_tasksExecutor->addLight(std::move(task));
}


int TasksQueue::heavyWorkers() const
{
    return m_tasksExecutor->heavyWorkers();
}


void TasksQueue::try_to_fire()
{
    std::lock_guard<std::recursive_mutex> guard(m_tasksMutex);

    // Do not put all waiting tasks to executor.
    while (m_maxTasks > m_executingTasks && m_waitingTasks.empty() == false)
    {
        fire();
    }
}


void TasksQueue::fire()
{
    std::lock_guard<std::recursive_mutex> guard(m_tasksMutex);
    assert(m_waitingTasks.empty() == false);

    auto task = m_mode == Mode::Fifo? take_front(m_waitingTasks): take_back(m_waitingTasks);

    m_executingTasks++;
    m_tasksExecutor->add(std::move(task));
}


void TasksQueue::task_finished()
{
    // It is possible that in this function TasksQueue::fire() will be on stack
    // of current thread (if m_tasksExecutor->add(std::move(task)); executes task immediately)
    // That's why we need recursive mutex here
    std::lock_guard<std::recursive_mutex> guard(m_tasksMutex);

    assert(m_executingTasks > 0);
    m_executingTasks--;

    if (m_executingTasks == 0)
        m_noWork.notify_one();

    try_to_fire();
}


std::unique_ptr<ITaskExecutor::ITask> inlineTask(const std::string& name, std::function<void()>&& task)
{
    return std::make_unique<InlineTask>(name, std::move(task));
}
