
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <Qt5/model_view/view_helpers/data.hpp>

#include "mock_configuration.hpp"


TEST(DataShould, BeConstructable)
{

    EXPECT_NO_THROW({
        MockConfiguration config;

        Data data;
        data.m_configuration = &config;
    });
}
 

TEST(DataShould, BeEmptyWhenConstructed)
{

    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    const auto& items = data.getAll();
    EXPECT_EQ(true, items.empty());
}


TEST(DataShould, AddNewItemWhenAskedAboutNotExistingOne)
{

    MockConfiguration config;

    Data data;
    data.m_configuration = &config;
    data.get(QModelIndex());

    const auto& items = data.getAll();
    EXPECT_EQ(1, items.size());
}


TEST(DataShould, ReturnExistingItemWhenItWasCreatedPreviously)
{

    MockConfiguration config;

    Data data;
    data.m_configuration = &config;
    data.get(QModelIndex());            //first access - new item
    data.get(QModelIndex());            //second access - the same item

    const auto& items = data.getAll();
    EXPECT_EQ(1, items.size());
}


TEST(DataShould, ForgetAboutItemWhenAskedForIt)
{
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;
    data.get(QModelIndex());            //first access - new item
    data.forget(QModelIndex());

    const auto& items = data.getAll();
    EXPECT_EQ(true, items.empty());
}


TEST(DataShould, ForgetAboutItemWhenAskedForIt7)
{
    using ::testing::_;
    using ::testing::Return;

    MockConfiguration config;

    Data data;
    data.m_configuration = &config;
    data.get(QModelIndex());            //first access - new item
    data.forget(QModelIndex());

    const auto& items = data.getAll();
    EXPECT_EQ(true, items.empty());
}
