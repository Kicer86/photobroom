
#include "observable_executor.hpp"

ObservableExecutor::ObservableExecutor()
{
}


int ObservableExecutor::awaitingTasks() const
{
    return m_awaitingTasks;
}


int ObservableExecutor::tasksExecuted() const
{
    return m_tasksExecuted;
}


void ObservableExecutor::newTaskInQueue()
{
    m_awaitingTasks++;

    emit awaitingTasksChanged(m_awaitingTasks);
}


void ObservableExecutor::taskMovedToExecution()
{
    m_awaitingTasks--;
    m_tasksExecuted++;

    emit awaitingTasksChanged(m_awaitingTasks);
    emit tasksExecutedChanged(m_tasksExecuted);
}


void ObservableExecutor::taskExecuted()
{
    m_tasksExecuted--;

    emit tasksExecutedChanged(m_tasksExecuted);
}
