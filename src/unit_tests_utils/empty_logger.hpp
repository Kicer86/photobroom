

#include <core/ilogger.hpp>

class EmptyLogger: public ILogger
{
    public:
        void log(Severity, const std::string &) override {}

        void info(const std::string &) override {}
        void warning(const std::string &) override {}
        void error(const std::string &) override {}
        void debug(const std::string &) override {}

        std::unique_ptr<ILogger> subLogger(const QString &)
        {
            return std::make_unique<EmptyLogger>();
        }
};
