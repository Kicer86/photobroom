#ifndef TASK_EXECUTOR_QCORO_UTILS_HPP_INCLUDED
#define TASK_EXECUTOR_QCORO_UTILS_HPP_INCLUDED


#include <type_traits>
#include <utility>

#include <QCoroFuture>
#include <QCoroTask>

#include "task_executor_utils.hpp"


template<typename Callable>
using run_on_result_t = std::remove_cvref_t<std::invoke_result_t<std::remove_reference_t<Callable>>>;


template<typename Callable>
QCoro::Task<run_on_result_t<Callable>>
runOnCoro(
    ITaskExecutor& executor,
    Callable&& callable,
    const std::string& taskName = std::source_location::current().function_name())
    requires std::is_invocable_v<std::remove_reference_t<Callable>>
{
    using Result = run_on_result_t<Callable>;

    auto future = runOn<Result>(
        executor,
        [callable = std::forward<Callable>(callable)](QPromise<Result>& promise) mutable
        {
            if constexpr (std::is_void_v<Result>)
            {
                callable();
            }
            else
            {
                Result result = callable();
                promise.addResult(std::move(result));
            }
        },
        taskName
    );

    if constexpr (std::is_void_v<Result>)
    {
        co_await future;
        co_return;
    }
    else
    {
        co_return co_await future;
    }
}

#endif
