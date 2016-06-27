
#ifndef ILOG_HPP
#define ILOG_HPP

#include <string>
#include <vector>

struct ILogger
{
    virtual ~ILogger() = default;

    enum class Severity
    {
        Error,
        Warning,
        Info,
        Debug,
    };

    enum Scope
    {
        Project = 1,
        Global  = 2,
    };

    virtual void log(Scope, Severity, const std::string& message) = 0;

    virtual void info(Scope, const std::string &) = 0;
    virtual void warning(Scope, const std::string &) = 0;
    virtual void error(Scope, const std::string &) = 0;
    virtual void debug(Scope, const std::string &) = 0;
};

#endif
