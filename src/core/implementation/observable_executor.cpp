
#include "observable_executor.hpp"
#include "observables_registry.hpp"


ObservableExecutor::ObservableExecutor()
{
    ObservablesRegistry::instance().add(this);

    m_executionSpeedBuffer.fill(0);

    connect(&m_executionSpeedTimer, &QTimer::timeout, this, &ObservableExecutor::updateExecutionSpeed);
    m_executionSpeedTimer.start(std::chrono::seconds(1));
}


ObservableExecutor::~ObservableExecutor()
{
    ObservablesRegistry::instance().remove(this);
}


int ObservableExecutor::awaitingTasks() const
{
    return m_awaitingTasks;
}


int ObservableExecutor::tasksExecuted() const
{
    return m_tasksExecuted;
}


double ObservableExecutor::executionSpeed() const
{
    return m_executionSpeed;
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
    m_executionSpeedCounter++;

    emit tasksExecutedChanged(m_tasksExecuted);
}


void ObservableExecutor::updateExecutionSpeed()
{
    const std::size_t bufferSize = m_executionSpeedBuffer.size();
    m_executionSpeedBuffer[m_executionSpeedBufferPos] = m_executionSpeedCounter;
    m_executionSpeedBufferPos = (m_executionSpeedBufferPos + 1) % bufferSize;
    m_executionSpeedCounter = 0;

    m_executionSpeed = std::accumulate(m_executionSpeedBuffer.begin(), m_executionSpeedBuffer.end(), 0) / static_cast<double>(bufferSize);

    emit executionSpeedChanged(m_executionSpeed);
}
