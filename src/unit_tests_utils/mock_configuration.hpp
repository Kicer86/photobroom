
#ifndef MOCK_CONFIGURATION_HPP_INCLUDED
#define MOCK_CONFIGURATION_HPP_INCLUDED

#include <gmock/gmock.h>

#include <core/iconfiguration.hpp>


class IConfigurationMock: public IConfiguration
{
    public:
        MOCK_METHOD(QVariant, getEntry, (const QString &), (override));
        MOCK_METHOD(void, setEntry, (const QString &, const QVariant &), (override));
        MOCK_METHOD(void, setDefaultValue, (const QString &, const QVariant &), (override));
        MOCK_METHOD(void, watchFor, (const QString& key, const Watcher &), (override));
};

#endif
