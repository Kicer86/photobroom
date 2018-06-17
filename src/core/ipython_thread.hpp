
#ifndef IPYTHON_THREAD_HPP
#define IPYTHON_THREAD_HPP


#include <functional>
#include <memory>

#include "core_export.h"

struct CORE_EXPORT IPythonThread
{
    struct ITask
    {
        virtual ~ITask() = default;
        virtual void run() = 0;
    };

    virtual ~IPythonThread() = default;
    virtual void execute(std::unique_ptr<ITask> &&) = 0;

    template<typename Callable>
    void execute(Callable&& c)
    {
        std::unique_ptr<ITask> task = std::make_unique<Task<Callable>>(std::forward<Callable>(c));
        execute(std::move(task));
    }

    protected:
        struct Impl;

        template<typename Callable>
        struct Task: ITask
        {
            Task(Callable&& c): m_c(std::forward<Callable>(c)) {}

            void run() override
            {
                m_c();
            }

            Callable m_c;
        };
};

#endif
