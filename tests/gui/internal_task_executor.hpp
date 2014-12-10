
#ifndef INTERNAL_TASK_EXECUTOR_HPP
#define INTERNAL_TASK_EXECUTOR_HPP

#include <core/itask_executor.hpp>

struct InternalTaskExecutor: ITaskExecutor
{
    virtual ~InternalTaskExecutor();
    
    virtual void add(const std::shared_ptr<ITask> &);
};

#endif
 
 
