
#ifndef MOCK_CONFIGURATION_HPP
#define MOCK_CONFIGURATION_HPP

#include <gmock/gmock.h>

#include <QString>
#include <QJsonValue>

#include <core/iconfiguration.hpp>

struct MockConfiguration: IConfiguration
{
    MOCK_METHOD(QVariant, getEntry, (const QString &), (override));

    MOCK_METHOD(void, setEntry, (const QString &, const QVariant &), (override));
    MOCK_METHOD(void, setDefaultValue, (const QString &, const QVariant &), (override));
    MOCK_METHOD(void, watchFor, (const QString &, const Watcher &), (override));
};

#endif
