
#include <cassert>

#include "task_executor_utils.hpp"
#include "containers_utils.hpp"


namespace
{
    class InlineTask: public ITaskExecutor::ITask
    {
        public:
            InlineTask(const std::string& name, std::function<void()>&& task)
                : m_name(name)
                , m_task(task)
            {

            }

            std::string name() const override
            {
                return m_name;
            }

            void perform() override
            {
                m_task();
            }

        private:
            const std::string m_name;
            std::function<void()> m_task;
    };
}


void WorkState::abort()
{
    m_abort = true;
}


void WorkState::throwIfAbort()
{
    if (m_abort)
        throw abort_exception();
}


struct TasksQueue::IntTask final: ITaskExecutor::ITask
{
    IntTask(std::unique_ptr<ITaskExecutor::ITask>&& callable, const Notifier& notifier):
        m_task(std::move(callable)),
        m_notifier(notifier)
    {

    }

    void perform() override
    {
        m_task->perform();
    }

    std::string name() const override
    {
        return std::string("TasksQueue::IntTask: ") + m_task->name();
    }

    std::unique_ptr<ITaskExecutor::ITask> m_task;
    std::shared_ptr<void> m_notifier;
};



TasksQueue::TasksQueue(ITaskExecutor& executor, Mode mode)
    : Queue(executor.heavyWorkers() * 3 / 2, mode)
    , m_executor(executor)
{

}


void TasksQueue::add(std::unique_ptr<ITask>&& task)
{
    push(std::move(task));
}


void TasksQueue::add(std::shared_ptr<IProcess>&& task)
{
    /// TODO: implement
    assert(!"Not implemenbted yet");
}


int TasksQueue::heavyWorkers() const
{
    return m_executor.heavyWorkers();
}


void TasksQueue::passTaskToExecutor(std::unique_ptr<ITask>&& task, const Notifier& notifier)
{
    m_executor.add(std::make_unique<IntTask>(std::move(task), notifier));
}


std::unique_ptr<ITaskExecutor::ITask> inlineTask(const std::string& name, std::function<void()>&& task)
{
    return std::make_unique<InlineTask>(name, std::move(task));
}
