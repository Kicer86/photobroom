
#ifndef MOCK_CONFIGURATION_HPP
#define MOCK_CONFIGURATION_HPP

#include <gmock/gmock.h>

#include <QString>

#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>

struct MockConfiguration: IConfiguration
{
    MOCK_METHOD1(addEntry, void(const Configuration::EntryData&));
    MOCK_CONST_METHOD2(findEntry, QString(const char*, const QString& defaultValue));
    MOCK_CONST_METHOD1(findEntry, Optional<Configuration::EntryData>(const Configuration::ConfigurationKey&));
    MOCK_CONST_METHOD1(findEntry, Optional<QString>(const char*));
    MOCK_METHOD0(getEntries, const std::vector<Configuration::EntryData>());
    MOCK_METHOD0(load, bool());
    MOCK_METHOD1(registerDefaultEntries, void(const std::vector<Configuration::EntryData>&));
    MOCK_METHOD1(registerKey, void(const Configuration::ConfigurationKey &));
    MOCK_METHOD1(registerXml, void(const QString &));
};

#endif
