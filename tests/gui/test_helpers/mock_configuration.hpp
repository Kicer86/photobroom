
#ifndef MOCK_CONFIGURATION_HPP
#define MOCK_CONFIGURATION_HPP

#include <gmock/gmock.h>

#include <QString>
#include <QJsonValue>

#include <configuration/iconfiguration.hpp>

struct MockConfiguration: IConfiguration
{
    MOCK_METHOD1(getEntry, ol::Optional<QVariant>(const QString &));
    MOCK_METHOD2(setEntry, void(const QString &, const QVariant &));
};

#endif
