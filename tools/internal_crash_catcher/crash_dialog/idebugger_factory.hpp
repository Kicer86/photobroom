
#ifndef IDEBUGGER_FACTORY_HPP
#define IDEBUGGER_FACTORY_HPP

#include <memory>

#include "idebugger.hpp"


struct IDebuggerFactory
{
    virtual ~IDebuggerFactory();

    virtual std::unique_ptr<IDebugger> get() = 0;
};

#endif
