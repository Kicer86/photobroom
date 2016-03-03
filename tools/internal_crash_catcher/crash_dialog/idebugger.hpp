
#ifndef IDEBUGGER_HPP
#define IDEBUGGER_HPP

#include <string>
#include <vector>
#include <functional>

#include <QString>

struct IDebugger
{
    virtual ~IDebugger();

    virtual bool init(qint64 pid, qint64 tid, const QString& exec) = 0;
    virtual void requestBacktrace( const std::function<void(const std::vector<QString> &)> & ) = 0;

    virtual const QString& exec() const = 0;
};

#endif // IDEBUGGER_HPP
