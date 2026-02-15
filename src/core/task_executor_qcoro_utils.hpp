#ifndef TASK_EXECUTOR_QCORO_UTILS_HPP_INCLUDED
#define TASK_EXECUTOR_QCORO_UTILS_HPP_INCLUDED


#include <source_location>
#include <type_traits>
#include <utility>

#include <QCoroCore>

#include "qcoro_task_utils.hpp"
#include "task_executor_utils.hpp"


template<typename Callable>
using co_run_on_result_t = std::remove_cvref_t<std::invoke_result_t<std::remove_reference_t<Callable>>>;


template<typename Callable>
QCoro::Task<co_run_on_result_t<Callable>> coRunOn(
    ITaskExecutor& executor,
    Callable&& callable,
    const std::string& taskName = std::source_location::current().function_name())
    requires std::is_invocable_v<std::remove_reference_t<Callable>>
{
    using Result = co_run_on_result_t<Callable>;
    co_return co_await QCoroTaskUtils::coRunScheduled<Result>(
        [&executor, taskName](auto&& task)
        {
            runOn(executor, std::forward<decltype(task)>(task), taskName);
        },
        std::forward<Callable>(callable)
    );
}

template<typename Callable, typename Receiver, typename Slot>
void coRunOn(
    ITaskExecutor& executor,
    Callable&& callable,
    Receiver* receiver,
    Slot slot,
    const std::string& taskName = std::source_location::current().function_name())
    requires std::is_invocable_v<std::remove_reference_t<Callable>>
{
    QCoro::connect(
        coRunOn(executor, std::forward<Callable>(callable), taskName),
        receiver, slot
    );
}

#endif
