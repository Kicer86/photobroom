
#ifndef MOCK_LOGGER_FACTORY_HPP_INCLUDED
#define MOCK_LOGGER_FACTORY_HPP_INCLUDED

#include <gmock/gmock.h>

#include <core/ilogger_factory.hpp>

class ILoggerFactoryMock: public ILoggerFactory
{
    public:
        MOCK_METHOD(std::unique_ptr<ILogger>, get, (const QString& utility), (const, override));
        MOCK_METHOD(std::unique_ptr<ILogger>, get, (const QStringList& utility), (const, override));
};


#endif
