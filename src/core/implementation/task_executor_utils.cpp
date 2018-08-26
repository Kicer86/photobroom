
#include "task_executor_utils.hpp"


TasksQueue::TasksQueue(ITaskExecutor* executor):
    m_tasksMutex(),
    m_waitingTasks(),
    m_tasksExecutor(executor),
    m_maxTasks(16),
    m_executingTasks(0)
{

}


void TasksQueue::clean()
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

    try_to_fire();
}
