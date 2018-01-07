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

#ifndef ITASKEXECUTOR_H
#define ITASKEXECUTOR_H

#include <memory>
#include <string>

#include "core_export.h"

struct CORE_EXPORT ITaskExecutor
{
    struct CORE_EXPORT ITask
    {
        virtual ~ITask();

        virtual std::string name() const = 0;       //task's name
        virtual void perform() = 0;
    };

    virtual ~ITaskExecutor();

    virtual void add(std::unique_ptr<ITask> &&) = 0;
    virtual void stop() = 0;
};


#endif // TASKEXECUTOR_H
