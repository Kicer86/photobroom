
#ifndef IDEBUGGER_HPP
#define IDEBUGGER_HPP

#include <string>
#include <vector>

#include <QtGlobal>

struct IDebugger
{
    virtual ~IDebugger();

    virtual bool attach(qint64 pid, qint64 tid) = 0;
    virtual std::vector<std::string> getBackTrace() = 0;
};

#endif // IDEBUGGER_HPP
