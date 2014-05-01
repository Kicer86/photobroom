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

#ifdef USE_OPENMP
    #include <omp.h>
#endif

#include <thread>

#include <OpenLibrary/palgorithm/ts_queue.hpp>
#include <OpenLibrary/palgorithm/ts_resource.hpp>


ITaskExecutor::ITask::~ITask()
{

}


ITaskExecutor::~ITaskExecutor()
{

}


///////////////////////////////////////////////////////////////////////////////


struct TaskExecutor;
static void trampoline(TaskExecutor *);

struct TaskExecutor: public ITaskExecutor
{
    TaskExecutor();
    virtual ~TaskExecutor();

    virtual void add(const std::shared_ptr<ITask> &);

    private:
        TS_Queue<std::shared_ptr<ITask>> m_tasks;
        std::thread m_taskEater;
        friend void trampoline(TaskExecutor* executor);

        void eat();
};


static void trampoline(TaskExecutor* executor)
{
    executor->eat();
}


TaskExecutor::TaskExecutor(): m_tasks(2048), m_taskEater(trampoline, this)
{

}


TaskExecutor::~TaskExecutor()
{
    m_tasks.break_popping();
    assert(m_taskEater.joinable());
    m_taskEater.join();
}


void TaskExecutor::add(const std::shared_ptr<ITask> &task)
{
    m_tasks.push_back(task);
}

//TODO: kill threads when no tasks
void TaskExecutor::eat()
{
    #pragma omp parallel
    {
#if USE_OPENMP
        const int id = omp_get_thread_num();
        **(ThreadSafeOutput.get()) << "Starting TaskExecutor thread #" << id << std::endl;
#endif

        while(true)
        {
            boost::optional<std::shared_ptr<ITask>> opt_task = m_tasks.pop_front();

            **(ThreadSafeOutput.get()) << "TaskExecutor thread #" << id
                                       << " takes task. " << m_tasks.size() << " tasks left"
                                       << std::endl;

            if (opt_task)
            {
                std::shared_ptr<ITask> task = *opt_task;

                const auto start = std::chrono::steady_clock::now();
                task->perform();
                const auto end = std::chrono::steady_clock::now();
                const auto diff = end - start;
                const auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
                **(ThreadSafeOutput.get()) << "Task's execution time: " << diff_ms << "ms" << std::endl;
            }
            else
                break;
        }

#ifdef USE_OPENMP
        **(ThreadSafeOutput.get()) << "Quitting TaskExecutor thread #" << id << std::endl;
#endif
    }
}



///////////////////////////////////////////////////////////////////////////////


ITaskExecutor* TaskExecutorConstructor::get()
{
    static TaskExecutor task_executor;

    return &task_executor;
}

