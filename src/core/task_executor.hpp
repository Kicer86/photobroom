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

#ifndef TASKEXECUTOR_HPP
#define TASKEXECUTOR_HPP

#include <thread>

#include "core_export.h"
#include "itask_executor.hpp"
#include "ts_queue.hpp"


struct ILogger;

struct CORE_EXPORT TaskExecutor: public ITaskExecutor
{
    explicit TaskExecutor(ILogger *);
    TaskExecutor(const TaskExecutor &) = delete;
    virtual ~TaskExecutor();

    TaskExecutor& operator=(const TaskExecutor &) = delete;

    void add(std::unique_ptr<ITask> &&) override;
    void addLight(std::unique_ptr<ITask> &&) override;

    int heavyWorkers() const override;

    void stop();

private:
    typedef ol::TS_Queue<std::unique_ptr<ITask>> QueueT;
    QueueT m_tasks;
    std::thread m_taskEater;
    std::mutex m_lightTasksMutex;
    std::condition_variable m_lightTaskFinished;
    ILogger* m_logger;
    unsigned int m_threads;
    int m_lightTasks;
    bool m_working;

    void eat();
    void execute(const std::shared_ptr<ITask>& task) const;
};


#endif // TASKEXECUTOR_HPP
