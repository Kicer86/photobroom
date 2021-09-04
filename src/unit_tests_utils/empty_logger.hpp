
#ifndef EMPTY_LOGGER_HPP
#define EMPTY_LOGGER_HPP

#include <core/ilogger.hpp>

class EmptyLogger final: public ILogger
{
    public:
        void log(Severity, const QString &) override {}

        void info(const QString &) override {}
        void warning(const QString &) override {}
        void error(const QString &) override {}
        void debug(const QString &) override {}
        void trace(const QString &) override {}

        std::unique_ptr<ILogger> subLogger(const QString &) override
        {
            return std::make_unique<EmptyLogger>();
        }
};

#endif
