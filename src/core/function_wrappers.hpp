
#ifndef CALLBACK_PTR_HPP
#define CALLBACK_PTR_HPP

#include <functional>
#include <memory>
#include <mutex>
#include <stop_token>

#include <QPointer>
#include <QPromise>
#include <QThread>


// extends QMetaObject::invokeMethod by version with arguments
template<typename Obj, typename F, typename... Args>
void invokeMethod(Obj* object, const F& method, Args&&... args) requires std::is_base_of<QObject, Obj>::value
{
    QMetaObject::invokeMethod(object, [object, method, ...args = std::forward<Args>(args)]() mutable
    {
        (object->*method)(std::forward<Args>(args)...);
    });
}


// like invokeMethod but postponed
template<typename Obj, typename F, typename... Args>
void invokeMethodLater(Obj* object, const F& method, Args&&... args) requires std::is_base_of<QObject, Obj>::value
{
    QMetaObject::invokeMethod(object, [object, method, ...args = std::forward<Args>(args)]() mutable
    {
        (object->*method)(std::forward<Args>(args)...);
    }, Qt::QueuedConnection);
}


// Works as extended invokeMethod but waits for results
template<typename T, typename ObjT, typename F, typename... Args>
requires std::is_base_of_v<QObject, ObjT>
auto invoke_and_wait(QPointer<ObjT> object, const F& function, Args&&... args)
{
    QPromise<T> promise;
    QFuture<T> future = promise.future();

    call_from_object_thread(object, [&promise, &function, &args...]()
    {
        promise.start();
        promise.addResult(function(args...));
        promise.finish();
    });

    future.waitForFinished();

    return future.result();
}


// call_from_object_thread uses Qt mechanisms to invoke function in another thread
// (thread of 'object' object)
template<typename F, typename ObjT, typename... Args>
requires std::is_base_of_v<QObject, ObjT>
void call_from_object_thread(QPointer<ObjT> object, const F& function, Args&&... args)
{
    if (object.data() != nullptr)
    {
        if constexpr (std::is_member_function_pointer_v<F>)
            QMetaObject::invokeMethod(object.data(), [object, function, args...]()
            {
                ((*object).*function)(args...);
            });
        else
            QMetaObject::invokeMethod(object.data(), [function, args...]()
            {
                function(args...);
            });
    }
}


template<typename F, typename... Args>
void call_from_this_thread(QThread* thread, const F& function, Args&&... args)
{
    QMetaObject::invokeMethod(thread, [function, args...]()
    {
        function(args...);
    });
}


// construct a functor which invoked will invoke encapsulated
// functor in another thread
template<typename... Args, typename ObjT, typename F>
requires std::is_base_of_v<QObject, ObjT>
[[nodiscard]] std::function<void(Args...)> make_cross_thread_function(ObjT* object, const F& function)
{
    std::function<void(Args...)> result = [=](Args&&... args)
    {
        call_from_object_thread(QPointer<ObjT>(object), function, std::forward<Args>(args)...);
    };

    return result;
}


template<typename... Args, typename F>
[[nodiscard]] std::function<void(Args...)> make_cross_thread_function(QThread* thread, const F& function)
{
    std::function<void(Args...)> result = [=](Args&&... args)
    {
        call_from_this_thread(thread, function, std::forward<Args>(args)...);
    };

    return result;
}


// construct a functor which invoked will invoke a method
// (slot) of given object. Will do nothing when given object is destroyed.
// Similar to safe_callback_ctrl (but method will be invoked in target's thread)
template<typename ObjT, typename R, typename ...Args>
requires std::is_base_of_v<QObject, ObjT>
[[nodiscard]] auto queued_slot(ObjT* obj, R(ObjT::*method)(Args...))
{
    QPointer<ObjT> objPtr(obj);

    return [objPtr, method](Args... args)
    {
        ObjT* object = objPtr.data();

        if (object)
            invokeMethod(object, method, std::forward<Args>(args)...);
    };
}

// construct a functor which invoked will invoke a method
// (slot) of given object. Will do nothing when given object is destroyed.
// Similar to safe_callback_ctrl (but uses Qt mechanism to guarantee (?) threadsafety)
// In contrast to queued_slot() method is invoked in caller's thread
template<typename ObjT, typename R, typename ...Args>
requires std::is_base_of_v<QObject, ObjT>
[[nodiscard]] std::function<void(Args...)> direct_slot(ObjT* obj, R(ObjT::*method)(Args...))
{
    QPointer<ObjT> objPtr(obj);

    return [objPtr, method](Args... args)
    {
        ObjT* object = objPtr.data();

        if (object)
            (object->*method)(std::forward<Args>(args)...);
    };
}

#endif
