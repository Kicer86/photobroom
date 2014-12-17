
#include "internal_task_executor.hpp"


InternalTaskExecutor::~InternalTaskExecutor()
{

}


void InternalTaskExecutor::add(const std::shared_ptr<ITaskExecutor::ITask>& task)
{
    //execute immediatelly
    task->perform();
}
