
#ifndef IDEBUGGER_HPP
#define IDEBUGGER_HPP

#include <string>
#include <vector>

#include <QString>

struct IDebugger
{
    virtual ~IDebugger();

    virtual bool attach(qint64 pid, qint64 tid, const QString& exec) = 0;
    virtual std::vector<QString> getBackTrace() = 0;
};

#endif // IDEBUGGER_HPP
