
#include <cassert>
#include <chrono>
#include <numeric>

#include <QHash>
#include <QVector>

#include <core/function_wrappers.hpp>

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
    return m_awaitingTasks.load();
}


int ObservableExecutor::tasksExecuted() const
{
    return m_tasksExecuted.load();
}


double ObservableExecutor::executionSpeed() const
{
    return m_executionSpeed;
}


const std::vector<ObservableExecutor::TaskEntry>& ObservableExecutor::tasks() const
{
    return m_taskEntries;
}


void ObservableExecutor::newTaskInQueue(const std::string& name)
{
    const QString qname = QString::fromStdString(name);

    m_awaitingTasks++;

    invokeMethod(this, &ObservableExecutor::notifyAwaitingTasksChanged);
    invokeMethod(this, &ObservableExecutor::adjustTask, qname, 1);
}


void ObservableExecutor::taskMovedToExecution(const std::string &)
{
    m_awaitingTasks--;
    m_tasksExecuted++;

    assert(m_awaitingTasks >= 0);

    invokeMethod(this, &ObservableExecutor::notifyAwaitingTasksChanged);
    invokeMethod(this, &ObservableExecutor::notifyTasksExecutedChanged);
}


void ObservableExecutor::taskExecuted(const std::string& name)
{
    const QString qname = QString::fromStdString(name);

    m_tasksExecuted--;
    m_executionSpeedCounter++;

    assert(m_tasksExecuted >= 0);

    invokeMethod(this, &ObservableExecutor::notifyTasksExecutedChanged);
    invokeMethod(this, &ObservableExecutor::adjustTask, qname, -1);
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


void ObservableExecutor::notifyAwaitingTasksChanged()
{
    emit awaitingTasksChanged(m_awaitingTasks.load());
}


void ObservableExecutor::notifyTasksExecutedChanged()
{
    emit tasksExecutedChanged(m_tasksExecuted.load());
}


void ObservableExecutor::adjustTask(const QString& name, int delta)
{
    const auto it = m_taskRowForText.constFind(name);
    const int existingRow = it == m_taskRowForText.constEnd()? -1 : *it;

    if (existingRow < 0)
    {
        assert(delta >= 0);
        const int row = m_taskEntries.size();

        emit taskAboutToBeInserted(row);
        m_taskEntries.push_back(TaskEntry{.name = name, .count = delta});
        m_taskRowForText.insert(name, row);
        emit taskInserted(row, name, delta);
    }
    else
    {
        TaskEntry& entry = m_taskEntries[existingRow];
        entry.count += delta;
        assert(entry.count >= 0);
        emit taskUpdated(existingRow, entry.name, entry.count);
    }
}
