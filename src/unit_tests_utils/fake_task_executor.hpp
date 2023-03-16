
#ifndef FAKE_TASK_EXECUTOR_HPP_INCLUDED
#define FAKE_TASK_EXECUTOR_HPP_INCLUDED

#include <core/itask_executor.hpp>

class FakeTaskExecutor: public ITaskExecutor
{
    public:
        void add(std::unique_ptr<ITask>&& task) override
        {
            task->perform();
        }

        IProcessControl* add(Process &&) override
        {
            /// TODO: implement
            assert(!"Not implemented yet");

            return nullptr;
        }

        int heavyWorkers() const override
        {
            return 1;
        }
};

#endif
