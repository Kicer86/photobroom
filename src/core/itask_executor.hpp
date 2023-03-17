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

#ifndef ITASKEXECUTOR_H
#define ITASKEXECUTOR_H

#include <coroutine>
#include <functional>
#include <memory>
#include <string>

#include "core_export.h"


struct CORE_EXPORT ITaskExecutor
{
    struct CORE_EXPORT ITask
    {
        virtual ~ITask() = default;

        virtual std::string name() const = 0;               ///< @return task's name
        virtual void perform() = 0;                         ///< @brief perform job
    };

    enum class ProcessStateRequest
    {
        Suspend,
        Run,
        Terminate,
    };

    enum class ProcessState
    {
        Suspended,
        Running,
    };

    struct ProcessCoroutine
    {
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        struct promise_type
        {
            ProcessStateRequest stateRequest = ProcessStateRequest::Run;

            ProcessCoroutine get_return_object()
            {
                return ProcessCoroutine(handle_type::from_promise(*this));
            }
            std::suspend_always initial_suspend() noexcept { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void return_void() { stateRequest = ProcessStateRequest::Terminate; }
            void unhandled_exception() {}
            std::suspend_always yield_value(ProcessStateRequest sr) { stateRequest = sr; return {}; }
        };

        ProcessCoroutine(handle_type h_): h(h_) {}

        handle_type h = nullptr;
        operator std::coroutine_handle<promise_type>() const { return h; }
        operator std::coroutine_handle<>() const { return h; }
    };

    using Process = std::function<ProcessCoroutine()>;

    struct IProcessControl
    {
        virtual void terminate() = 0;
        virtual void resume() = 0;
        virtual ProcessState state() = 0;
    };

    virtual ~ITaskExecutor() = default;

    virtual void add(std::unique_ptr<ITask> &&) = 0;            // add short but heavy task (calculations)
    virtual IProcessControl* add(Process &&) = 0;               // add task to be run in a ring with other Processes

    virtual int heavyWorkers() const = 0;                       // return number of heavy task workers
};

#endif
