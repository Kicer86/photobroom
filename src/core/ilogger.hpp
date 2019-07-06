
#ifndef ILOG_HPP
#define ILOG_HPP

#include <memory>
#include <sstream>

#include "core_export.h"

class QString;

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

    virtual void log(Severity, const std::string& message) = 0;

    virtual void info(const std::string &) = 0;
    virtual void warning(const std::string &) = 0;
    virtual void error(const std::string &) = 0;
    virtual void debug(const std::string &) = 0;

    virtual std::unique_ptr<ILogger> subLogger(const QString& sub_utility) = 0;
};


template<ILogger::Severity severity>
class LoggerStream: std::stringbuf, public std::ostream
{
    public:
        explicit LoggerStream(ILogger* logger): std::stringbuf(), std::ostream(this), m_logger(logger)
        {

        }

        LoggerStream(const LoggerStream<severity> &) = delete;
        LoggerStream& operator=(const LoggerStream<severity> &) = delete;

        ~LoggerStream()
        {
            const std::string str = std::stringbuf::str();
            m_logger->log(severity, str);
        }

    private:
        ILogger* m_logger;
};

struct CORE_EXPORT InfoStream: LoggerStream<ILogger::Severity::Info>
{
    explicit InfoStream(ILogger *);
};

struct CORE_EXPORT WarningStream: LoggerStream<ILogger::Severity::Warning>
{
    explicit WarningStream(ILogger *);
};

struct CORE_EXPORT ErrorStream: LoggerStream<ILogger::Severity::Error>
{
    explicit ErrorStream(ILogger *);
};

struct CORE_EXPORT DebugStream: LoggerStream<ILogger::Severity::Debug>
{
    explicit DebugStream(ILogger *);
};

CORE_EXPORT std::ostream& operator<<(std::ostream& os, const QString& str);

#endif
