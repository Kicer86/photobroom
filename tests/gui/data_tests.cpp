
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
    data.get(QModelIndex());            // first access - new item
    data.forget(QModelIndex());         // forget about it

    const auto& items = data.getAll();
    EXPECT_EQ(true, items.empty());
}


TEST(DataShould, SetInitialDataForItems)
{
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    ModelIndexInfo info = data.get(QModelIndex());
    EXPECT_EQ(QModelIndex(), info.index);
    EXPECT_EQ(false, info.expanded);
    EXPECT_EQ(QRect(), info.getRect());
    EXPECT_EQ(QRect(), info.getOverallRect());
    EXPECT_EQ(false, info.isVisible());
}


TEST(DataShould, StoreInfoAboutItem)
{
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    ModelIndexInfo info = data.get(QModelIndex());
    info.expanded = true;
    info.setRect(QRect(0, 0, 100, 50));
    info.setOverallRect(QRect(0, 0, 100, 50));
    data.update(info);

    const ModelIndexInfo info2 = data.get(QModelIndex());
    EXPECT_EQ(true, info2.expanded);
    EXPECT_EQ(QRect(0, 0, 100, 50), info2.getRect());
    EXPECT_EQ(QRect(0, 0, 100, 50), info2.getOverallRect());
}


TEST(DataShould, AllowToHideItems)
{
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    ModelIndexInfo info = data.get(QModelIndex());
    info.markInvisible();
    EXPECT_EQ(false, info.expanded);
    info.expanded = true;
    data.update(info);

    const ModelIndexInfo info2 = data.get(QModelIndex());
    EXPECT_EQ(true, info2.expanded);
}
