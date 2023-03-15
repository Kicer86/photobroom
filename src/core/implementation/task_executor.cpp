/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "task_executor.hpp"
#include <ilogger.hpp>

#include <chrono>
#include <iostream>
#include <thread>

#include <QString>

#include "thread_utils.hpp"


TaskExecutor::TaskExecutor(ILogger& logger, int threadsToUse):
    m_tasks(),
    m_taskEater(),
    m_logger(logger),
    m_threads(threadsToUse),
    m_working(true)
{
    m_logger.info(QString("Using %1 threads.").arg(m_threads));

    m_taskEater = std::thread([&]
    {
        this->eat();
    });

    m_processRunner = std::thread([&]
    {
        this->runProcesses();
    });
}


TaskExecutor::~TaskExecutor()
{
    stop();
}


void TaskExecutor::add(std::unique_ptr<ITask>&& task)
{
    assert(m_working);
    m_tasks.push(std::move(task));
}


void TaskExecutor::add(Process&& task)
{
    m_processes.emplace_back(ProcessInfo::State::Running, task());
    m_processesIdleCV.notify_one();
}


int TaskExecutor::heavyWorkers() const
{
    return m_threads;
}


void TaskExecutor::stop()
{
    if (m_working)
    {
        m_working = false;

        // stop processes
        m_processesIdleCV.notify_one();
        assert(m_processRunner.joinable());

        // stop heavy tasks
        m_tasks.stop();
        assert(m_taskEater.joinable());

        // wait for threads
        m_processRunner.join();
        m_taskEater.join();
    }
}


void TaskExecutor::eat()
{
    set_thread_name("TE::eater");

    using namespace std::chrono_literals;

    std::atomic<unsigned int> running_tasks(0);
    std::condition_variable free_worker;
    std::mutex free_worker_mutex;
    int id = 0;

    while(m_working)
    {
        //wait for any data in queue
        m_tasks.wait_for_data();

        // if there are tasks and a free worker - give him a job
        if (running_tasks < m_threads && m_tasks.empty() == false)
        {
            ++running_tasks;

            std::thread thread([&, th_id = ++id]
            {

                const QString loggerName = QString("Thread #%1").arg(th_id);
                auto threadLogger = m_logger.subLogger(loggerName);

                set_thread_name("TE::HeavyTask");

                threadLogger->debug("Starting TaskExecutor thread");

                while(true)
                {
                    std::optional<std::unique_ptr<ITask>> opt_task(m_tasks.pop_for(2000ms));
                    assert(opt_task.has_value() == false || opt_task->get() != nullptr);

                    if (opt_task)
                    {
                        std::unique_ptr<ITask> task = std::move(*opt_task);

                        assert(task.get() != nullptr);

                        const std::string taskName = task->name();

                        const auto start = std::chrono::steady_clock::now();
                        execute(std::move(task));
                        const auto end = std::chrono::steady_clock::now();

                        threadLogger->trace(
                            QString("task '%1' took %2ms")
                                .arg(taskName.c_str())
                                .arg(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())
                        );
                    }
                    else
                        break;
                }

                --running_tasks;
                threadLogger->debug("Quitting TaskExecutor thread");

                // notify manager that thread is gone
                free_worker.notify_one();
            });

            thread.detach();
        }

        //wait for all workers to be free
        std::unique_lock<std::mutex> free_worker_lock(free_worker_mutex);
        free_worker.wait(free_worker_lock, [&]
        {
            return running_tasks < m_threads;
        });
    }

    //wait for all workers
    std::unique_lock<std::mutex> workers_lock(free_worker_mutex);
    free_worker.wait(workers_lock, [&]
    {
        return running_tasks == 0;
    });

    m_logger.info("TaskExecutor: shutting down.");
}


void TaskExecutor::execute(const std::shared_ptr<ITask>& task) const
{
    task->perform();
}


void TaskExecutor::runProcesses()
{
    while(m_working)
    {
        bool has_running = false;

        for(auto& process: m_processes)
        {
            if (process.state == ProcessInfo::State::Running)
            {
                has_running = true;
                process.coru.h();
            }
        }

        if (has_running == false)
        {
            std::unique_lock lock(m_processesIdleMutex);
            m_processesIdleCV.wait(lock);
        }
    };
}
