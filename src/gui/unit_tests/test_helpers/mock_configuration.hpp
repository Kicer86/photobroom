
#ifndef MOCK_CONFIGURATION_HPP
#define MOCK_CONFIGURATION_HPP

#include <gmock/gmock.h>

#include <QString>
#include <QJsonValue>

#include <core/iconfiguration.hpp>

struct MockConfiguration: IConfiguration
{
    MOCK_METHOD1(getEntry, QVariant(const QString &));

    MOCK_METHOD2(setEntry, void(const QString &, const QVariant &));
    MOCK_METHOD2(setDefaultValue, void(const QString &, const QVariant &));
    MOCK_METHOD1(registerObserver, void(IConfigObserver *));
};

#endif
