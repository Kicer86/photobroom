
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>
#include <configuration/constants.hpp>

#include "positions_calculator.hpp"
#include "data.hpp"

namespace
{
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
}


TEST(PositionsCalculatorShould, BeConstructable)
{
    using ::testing::_;
    using ::testing::Return;

    //EXPECT_NO_THROW({
        MockConfiguration config;

        EXPECT_CALL(config, findEntry(Configuration::BasicKeys::thumbnailWidth, _)).Times(1).WillOnce(Return("20"));

        QStandardItemModel model;
        Data data;
        data.m_configuration = &config;

        PositionsCalculator calculator(&model, &data, 100);
    //});
}
 
