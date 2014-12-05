
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <Qt5/model_view/view_helpers/data.hpp>

#include "mock_configuration.hpp"


TEST(DataShould, BeConstructable)
{
    using ::testing::_;
    using ::testing::Return;

    EXPECT_NO_THROW({
        MockConfiguration config;

        Data data;
        data.m_configuration = &config;
    });
}
 

TEST(DataShould, BeEmptyWhenConstructed)
{
    using ::testing::_;
    using ::testing::Return;

    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    const auto& items = data.getAll();
    EXPECT_EQ(0, items.size());
}


TEST(DataShould, AddNewItemWhenAskedAboutNotExistingOne)
{
    using ::testing::_;
    using ::testing::Return;

    MockConfiguration config;

    Data data;
    data.m_configuration = &config;
    data.get(QModelIndex());

    const auto& items = data.getAll();
    EXPECT_EQ(1, items.size());
}


TEST(DataShould, ReturnExistingItemWhenItWasCreatedPreviously)
{
    using ::testing::_;
    using ::testing::Return;

    MockConfiguration config;

    Data data;
    data.m_configuration = &config;
    data.get(QModelIndex());            //first access - new item
    data.get(QModelIndex());            //second access - the same item

    const auto& items = data.getAll();
    EXPECT_EQ(1, items.size());
}
