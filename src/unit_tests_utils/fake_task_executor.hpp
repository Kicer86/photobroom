
#include <core/itask_executor.hpp>

class FakeTaskExecutor: public ITaskExecutor
{
    public:
        void add(std::unique_ptr<ITask>&& task) override
        {
            task->perform();
        }

        int heavyWorkers() const override
        {
            return 1;
        }
};
