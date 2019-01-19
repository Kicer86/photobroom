
#include <cassert>

#include "task_executor_utils.hpp"


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



TasksQueue::TasksQueue(ITaskExecutor* executor):
    m_tasksMutex(),
    m_waitingTasks(),
    m_tasksExecutor(executor),
    m_maxTasks(executor->heavyWorkers() + 2),
    m_executingTasks(0)
{

}


TasksQueue::~TasksQueue()
{
    clear();  // drop all tasks awaiting

    // wait for tasks being executed
    std::unique_lock<std::mutex> lock(m_tasksMutex);
    m_noWork.wait(lock, [this]
    {
        return m_executingTasks == 0;
    });
}


void TasksQueue::push(std::unique_ptr<ITaskExecutor::ITask>&& callable)
{
    std::lock_guard<std::mutex> guard(m_tasksMutex);

    auto task = std::make_unique<IntTask>(std::move(callable), this);
    m_waitingTasks.push_back(std::move(task));

    try_to_fire();
}


void TasksQueue::clear()
{
    std::lock_guard<std::mutex> guard(m_tasksMutex);
    m_waitingTasks.clear();
}


void TasksQueue::try_to_fire()
{
    assert(m_tasksMutex.try_lock() == false);  // here we expect mutex to be locked

    // Do not put all waiting tasks to executor.
    while (m_maxTasks > m_executingTasks && m_waitingTasks.empty() == false)
    {
        fire();
    }
}


void TasksQueue::fire()
{
    assert(m_tasksMutex.try_lock() == false);  // here we expect mutex to be locked
    assert(m_waitingTasks.empty() == false);

    auto task = std::move(m_waitingTasks.front());
    m_waitingTasks.pop_front();

    m_tasksExecutor->add(std::move(task));
    m_executingTasks++;
}


void TasksQueue::task_finished()
{
    std::lock_guard<std::mutex> guard(m_tasksMutex);

    assert(m_executingTasks > 0);
    m_executingTasks--;

    if (m_executingTasks == 0)
        m_noWork.notify_one();

    try_to_fire();
}
