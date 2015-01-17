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

#ifndef TASKEXECUTOR_H
#define TASKEXECUTOR_H

#include <thread>

#include <OpenLibrary/putils/ts_queue.hpp>

#include "itask_executor.hpp"
#include "core_export.h"

struct CORE_EXPORT TaskExecutor: public ITaskExecutor
{
    TaskExecutor();
    virtual ~TaskExecutor();

    virtual void add(const std::shared_ptr<ITask> &);
    virtual void stop();

    void eat();

private:
    ol::TS_Queue<std::shared_ptr<ITask>> m_tasks;
    std::thread m_taskEater;
    bool m_working;

    void execute(const std::shared_ptr<ITask>& task) const;
    int getId() const;
};


#endif // TASKEXECUTOR_H
