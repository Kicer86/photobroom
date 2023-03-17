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

#include <condition_variable>
#include <map>
#include <thread>

#include "core_export.h"
#include "itask_executor.hpp"
#include "ts_queue.hpp"


struct ILogger;

struct CORE_EXPORT TaskExecutor: public ITaskExecutor
{
    explicit TaskExecutor(ILogger &, int threadsToUse);
    TaskExecutor(const TaskExecutor &) = delete;
    virtual ~TaskExecutor();

    TaskExecutor& operator=(const TaskExecutor &) = delete;

    void add(std::unique_ptr<ITask> &&) override;
    IProcessControl* add(Process &&) override;

    int heavyWorkers() const override;

    void stop();

private:
    class ProcessInfo: IProcessControl
    {
    public:
        ProcessInfo(TaskExecutor& executor, ProcessState s, const ProcessCoroutine& h)
            : m_state(s)
            , m_co_h(h)
            , m_executor(executor)
        {}

        ~ProcessInfo()
        {
            m_co_h.destroy();
        }

        void terminate() override;
        void resume() override;
        ProcessState state() override;

        void setState(ProcessState s)
        {
            m_state = s;
        }

        ProcessStateRequest run() const
        {
            m_co_h();

            return stateRequest();
        }

        ProcessStateRequest stateRequest() const
        {
            const auto &promise = m_co_h.promise();
            return promise.stateRequest;
        }

    private:
        ProcessState m_state = ProcessState::Suspended;
        ProcessCoroutine::handle_type m_co_h;
        TaskExecutor& m_executor;
    };

    friend class ProcessInfo;

    typedef ol::TS_Queue<std::unique_ptr<ITask>> QueueT;
    QueueT m_tasks;
    std::vector<std::unique_ptr<ProcessInfo>> m_processes;
    std::thread m_taskEater;
    std::thread m_processRunner;
    std::mutex m_processesIdleMutex;
    std::mutex m_processAlternationMutex;
    std::condition_variable m_processesIdleCV;
    ILogger& m_logger;
    unsigned int m_threads;
    bool m_working;

    void eat();
    void execute(const std::shared_ptr<ITask>& task) const;
    void runProcesses();
    void terminate(ProcessInfo *);
    void wakeUpScheduler();
};


#endif
