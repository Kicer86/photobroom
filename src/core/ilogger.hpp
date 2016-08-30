
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

    virtual void log(Severity, const QString& message) = 0;

    virtual void info(const QString &) = 0;
    virtual void warning(const QString &) = 0;
    virtual void error(const QString &) = 0;
    virtual void debug(const QString &) = 0;
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
            std::string str = std::stringbuf::str();
            m_logger->log(severity, str.c_str());
        }

    private:
        ILogger* m_logger;
};

#endif
