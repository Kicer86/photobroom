
#ifndef IPYTHON_THREAD_HPP
#define IPYTHON_THREAD_HPP


#include <functional>


struct IPythonThread
{
    virtual ~IPythonThread() = default;
    virtual void execute(const std::function<void()> &) = 0;
};

#endif
