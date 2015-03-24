
#ifndef ILOG_HPP
#define ILOG_HPP

#include <string>
#include <vector>
 
struct ILogger
{
    virtual ~ILogger() {}

    enum class Severity
    {
        Error,
        Warning,
        Info,
        Debug,
    };

    virtual void log(Severity, const std::string& message) = 0;

    virtual void info(const std::string &) = 0;
    virtual void warning(const std::string &) = 0;
    virtual void error(const std::string &) = 0;
    virtual void debug(const std::string &) = 0;
};

#endif
