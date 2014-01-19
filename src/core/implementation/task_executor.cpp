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

ITaskExecutor::ITask::~ITask()
{

}


ITaskExecutor::~ITaskExecutor()
{

}


///////////////////////////////////////////////////////////////////////////////


struct TaskExecutor: public ITaskExecutor
{
    TaskExecutor();
    virtual ~TaskExecutor();

    virtual void add(ITask *);
};


TaskExecutor::TaskExecutor()
{

}


TaskExecutor::~TaskExecutor()
{

}


void TaskExecutor::add(ITaskExecutor::ITask*)
{

}


///////////////////////////////////////////////////////////////////////////////


ITaskExecutor* TaskExecutorConstructor::get()
{
    static TaskExecutor task_executor;

    return &task_executor;
}

