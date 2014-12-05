
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <Qt5/model_view/view_helpers/data.hpp>

#include "mock_configuration.hpp"
#include "mock_qabstractitemmodel.hpp"


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


TEST(DataShould, MarkItemsVisibleWhenTheyGetSize)
{
    MockQAbstractItemModel model;
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    QModelIndex top = model.createIndex(0, 0, &data);

    ModelIndexInfo info = data.get(top);
    info.setRect(QRect(0, 0, 100, 50));
    data.update(info);

    const ModelIndexInfo info2 = data.get(top);
    EXPECT_EQ(true, info2.isVisible());
}


TEST(DataShould, ReturnEmptySizesWhenInvisible)
{
    MockQAbstractItemModel model;
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    QModelIndex top = model.createIndex(0, 0, &data);

    ModelIndexInfo info = data.get(top);
    info.setRect(QRect(0, 0, 100, 50));
    info.setOverallRect(QRect(0, 0, 200, 150));
    data.update(info);

    info.markInvisible();
    data.update(info);

    const ModelIndexInfo info2 = data.get(top);
    EXPECT_EQ(false, info2.isVisible());
    EXPECT_EQ(QRect(), info2.getRect());
    EXPECT_EQ(QRect(), info2.getOverallRect());
}



TEST(DataShould, NotForgetItemSizeWhenItsMarkedInvisible)
{
    MockQAbstractItemModel model;
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    QModelIndex top = model.createIndex(0, 0, &data);

    ModelIndexInfo info = data.get(top);
    info.setRect(QRect(0, 0, 100, 50));
    info.setOverallRect(QRect(0, 0, 200, 150));
    data.update(info);

    info.markInvisible();
    data.update(info);

    info.markVisible();
    data.update(info);

    const ModelIndexInfo info2 = data.get(top);
    EXPECT_EQ(true, info2.isVisible());
    EXPECT_EQ(QRect(0, 0, 100, 50), info2.getRect());
    EXPECT_EQ(QRect(0, 0, 200, 150), info2.getOverallRect());
}
