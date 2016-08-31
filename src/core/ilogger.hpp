
#ifndef ILOG_HPP
#define ILOG_HPP

#include <sstream>

#include <QString>

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
};


template<ILogger::Severity severity>
class LoggerStream: std::stringbuf, public std::ostream
{
    public:
        LoggerStream(ILogger* logger): std::stringbuf(), std::ostream(this), m_logger(logger)
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

struct InfoStream: LoggerStream<ILogger::Severity::Info>
{
    InfoStream(ILogger* logger): LoggerStream(logger) {}
};

struct WarningStream: LoggerStream<ILogger::Severity::Warning>
{
    WarningStream(ILogger* logger): LoggerStream(logger) {}
};

struct ErrorStream: LoggerStream<ILogger::Severity::Error>
{
    ErrorStream(ILogger* logger): LoggerStream(logger) {}
};

struct DebugStream: LoggerStream<ILogger::Severity::Debug>
{
    DebugStream(ILogger* logger): LoggerStream(logger) {}
};

inline std::ostream& operator<<(std::ostream& os, const QString& str)
{
    os << str.toStdString();

    return os;
}

#endif
