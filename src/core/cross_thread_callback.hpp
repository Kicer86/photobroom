
#ifndef CROSS_THREAD_CALLBACK
#define CROSS_THREAD_CALLBACK

// http://stackoverflow.com/questions/21646467/how-to-execute-a-functor-in-a-given-thread-in-qt-gcd-style

#include <functional>

#include <QObject>

namespace FunctorCallConsumer
{
    QObject * forThread(QThread*);
}

template<typename... Args>
std::function<void(Args...)> cross_thread_function(QObject* object, const std::function<void(Args...)>& function)
{
    std::function<void(Args...)> result = [=](Args... args)
    {
        QObject signalSource;
        QObject::connect(&signalSource, &QObject::destroyed,
                         FunctorCallConsumer::forThread(object->thread()), [=](QObject *){ function(args...); });
    };

    return result;
}


#endif
