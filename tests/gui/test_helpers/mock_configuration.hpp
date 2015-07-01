
#ifndef MOCK_CONFIGURATION_HPP
#define MOCK_CONFIGURATION_HPP

#include <gmock/gmock.h>

#include <QString>
#include <QJsonValue>

#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>

struct MockConfiguration: IConfiguration
{
    MOCK_METHOD1(addEntry, void(const Configuration2::EntryData&));
    MOCK_CONST_METHOD2(findEntry, QString(const char*, const QString& defaultValue));
    MOCK_CONST_METHOD1(findEntry, ol::Optional<Configuration2::EntryData>(const Configuration2::ConfigurationKey&));
    MOCK_CONST_METHOD1(findEntry, ol::Optional<QString>(const char*));
    MOCK_METHOD0(getEntries, const std::vector<Configuration2::EntryData>());
    MOCK_METHOD0(load, bool());
    MOCK_METHOD1(registerDefaultEntries, void(const std::vector<Configuration2::EntryData>&));
    MOCK_METHOD1(registerKey, void(const Configuration2::ConfigurationKey &));
    MOCK_METHOD1(registerXml, void(const QString &));
    MOCK_METHOD1(getEntry, ol::Optional<QVariant>(const QString &));
    MOCK_METHOD2(setEntry, void(const QString &, const QVariant &));
};

#endif
