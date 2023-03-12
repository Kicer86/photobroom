
#include <core/itask_executor.hpp>

class FakeTaskExecutor: public ITaskExecutor
{
    public:
        void add(std::unique_ptr<ITask>&& task) override
        {
            task->perform();
        }

        void add(std::shared_ptr<IProcess>&& task) override
        {
            /// TODO: implement
            assert(!"Not implemented yet");
        }

        int heavyWorkers() const override
        {
            return 1;
        }
};
