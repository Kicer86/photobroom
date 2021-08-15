
#ifndef OBSERVABLE_EXECUTOR_HPP_INCLUDED
#define OBSERVABLE_EXECUTOR_HPP_INCLUDED

#include <QObject>
#include <core/itask_executor.hpp>

template<typename T> requires std::is_base_of<ITaskExecutor, T>::value
class ObservableExecutor: public QObject, public T
{
    public:
        template<typename ...Args>
        ObservableExecutor(Args... args): T(args...) {}

        void add(std::unique_ptr<ITaskExecutor::ITask>&& task) override
        {
            T::add(std::move(task));
        }

        void addLight(std::unique_ptr<ITaskExecutor::ITask>&& task) override
        {
            T::addLight(std::move(task));
        }
};

#endif
