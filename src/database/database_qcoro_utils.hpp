#ifndef DATABASE_QCORO_UTILS_HPP_INCLUDED
#define DATABASE_QCORO_UTILS_HPP_INCLUDED


#include <source_location>
#include <string>
#include <type_traits>
#include <utility>

#include <QCoroTask>

#include <core/qcoro_task_utils.hpp>

#include "idatabase.hpp"


namespace Database
{
    template<typename Callable>
    using co_run_on_result_t = std::remove_cvref_t<std::invoke_result_t<std::remove_reference_t<Callable>, IBackend&>>;


    template<typename Callable>
    QCoro::Task<co_run_on_result_t<Callable>>
    coRunOn(
        IDatabase& database,
        Callable&& callable,
        const std::string& taskName = std::source_location::current().function_name())
        requires std::is_invocable_v<std::remove_reference_t<Callable>, IBackend&>
    {
        using Result = co_run_on_result_t<Callable>;
        co_return co_await QCoroTaskUtils::coRunScheduled<Result>(
            [&database, &taskName](auto&& task)
            {
                database.exec(std::forward<decltype(task)>(task), taskName);
            },
            std::forward<Callable>(callable)
        );
    }


    template<typename Callable>
    QCoro::Task<co_run_on_result_t<Callable>>
    coExec(
        IDatabase& database,
        Callable&& callable,
        const std::string& taskName = std::source_location::current().function_name())
        requires std::is_invocable_v<std::remove_reference_t<Callable>, IBackend&>
    {
        co_return co_await coRunOn(database, std::forward<Callable>(callable), taskName);
    }
}

#endif
