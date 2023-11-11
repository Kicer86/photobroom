
#ifndef ILOG_HPP
#define ILOG_HPP

#include <memory>
#include <sstream>
#include <QString>


#include "core_export.h"

struct ILogger
{
    virtual ~ILogger() = default;

    enum class Severity
    {
        Error,
        Warning,
        Info,
        Debug,
        Trace,
    };

    virtual void log(Severity, const QString& message) = 0;

    virtual void info(const QString &) = 0;
    virtual void warning(const QString &) = 0;
    virtual void error(const QString &) = 0;
    virtual void debug(const QString &) = 0;
    virtual void trace(const QString &) = 0;

    virtual std::unique_ptr<ILogger> subLogger(const QString& sub_utility) const = 0;
};

#endif
