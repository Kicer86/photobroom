/*
 * An aplication wide universal task executor. Runs on all available cpu cores
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

#include <OpenLibrary/putils/ts_queue.hpp>
#include <OpenLibrary/putils/ts_resource.hpp>


ITaskExecutor::ITask::~ITask()
{

}


ITaskExecutor::~ITaskExecutor()
{

}


///////////////////////////////////////////////////////////////////////////////


TaskExecutor::TaskExecutor(ILogger* logger): m_tasks(), m_producer(), m_taskEater(), m_logger(logger), m_working(true)
{
    m_producer = m_tasks.prepareProducer();

    m_taskEater = std::thread( [&]
    {
        this->eat();
    });
}


TaskExecutor::~TaskExecutor()
{
    stop();
}


void TaskExecutor::add(std::unique_ptr<ITask>&& task)
{
    assert(m_working);
    m_producer->push(std::move(task));
}


TaskExecutor::TaskQueue TaskExecutor::getCustomTaskQueue()
{
    return m_tasks.prepareProducer();
}


void TaskExecutor::stop()
{
    if (m_working)
    {
        m_working = false;
        m_tasks.stop();
        assert(m_taskEater.joinable());
        m_taskEater.join();
    }
}


void TaskExecutor::eat()
{
    using namespace std::chrono_literals;

    const unsigned int threads = std::thread::hardware_concurrency();

    DebugStream(m_logger) << "TaskExecutor: " << threads << " threads detected.";

    std::atomic<unsigned int> running_tasks(0);
    std::condition_variable free_worker;
    std::mutex free_worker_mutex;

    while(m_working)
    {
        //wait for any data in queue
        m_tasks.wait_for_data();

        // if there are tasks and a free worker - give him a job
        if (running_tasks < threads && m_tasks.empty() == false)
        {
            ++running_tasks;

            std::thread thread([&]
            {
                std::thread::id id = std::this_thread::get_id();
                LoggerStream<ILogger::Severity::Debug>(m_logger) << "Starting TaskExecutor thread #" << id;

                while(true)
                {
                    std::optional<std::unique_ptr<ITask>> opt_task(m_tasks.pop_for(2000ms));
                    assert(opt_task.has_value() == false || opt_task->get() != nullptr);

                    if (opt_task)
                    {
                        std::unique_ptr<ITask> task = std::move(*opt_task);

                        assert(task.get() != nullptr);

                        execute(std::move(task));
                    }
                    else
                        break;
                }

                --running_tasks;
                DebugStream(m_logger) << "Quitting TaskExecutor thread #" << id;

                // notify manager that thread is gone
                free_worker.notify_one();
            });

            thread.detach();
        }

        //wait for all workers to be free
        std::unique_lock<std::mutex> free_worker_lock(free_worker_mutex);
        free_worker.wait(free_worker_lock, [&]
        {
            return running_tasks < threads;
        });
    }

    //wait for all workers
    std::unique_lock<std::mutex> workers_lock(free_worker_mutex);
    free_worker.wait(workers_lock, [&]
    {
        return running_tasks == 0;
    });

    DebugStream(m_logger) << "TaskExecutor: shutting down.";
}


void TaskExecutor::execute(const std::shared_ptr<ITask>& task) const
{
    task->perform();
}
