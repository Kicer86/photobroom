#ifndef QCORO_TASK_UTILS_HPP_INCLUDED
#define QCORO_TASK_UTILS_HPP_INCLUDED


#include <functional>
#include <type_traits>
#include <utility>

#include <QCoroFuture>
#include <QCoroTask>
#include <QFuture>
#include <QPromise>


namespace QCoroTaskUtils
{
    template<typename Result, typename Scheduler, typename Callable>
    QCoro::Task<Result> coRunScheduled(Scheduler&& scheduler, Callable&& callable)
    {
        QPromise<Result> promise;
        promise.start();
        auto future = promise.future();

        std::forward<Scheduler>(scheduler)(
            [callable = std::forward<Callable>(callable), promise = std::move(promise)](auto&&... args) mutable
            {
                if constexpr (std::is_void_v<Result>)
                {
                    std::invoke(callable, std::forward<decltype(args)>(args)...);
                }
                else
                {
                    Result result = std::invoke(callable, std::forward<decltype(args)>(args)...);
                    promise.addResult(std::move(result));
                }

                promise.finish();
            }
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
}

#endif
