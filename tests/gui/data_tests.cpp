
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <Qt5/model_view/view_helpers/data.hpp>
#include <Qt5/model_view/view_helpers/positions_calculator.hpp>

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


TEST(DataShould, ReturnEmptyInfoStructWhenAskedAboutNotExistingItem)
{
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;
    auto info = data.get(QModelIndex());

    EXPECT_EQ(info.index, QModelIndex());

    const auto& items = data.getAll();
    EXPECT_EQ(true, items.empty());
}


TEST(DataShould, StoreDataItemOnUpdate)
{
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;
    auto info = data.get(QModelIndex());
    data.update(info);

    const auto& items = data.getAll();
    EXPECT_EQ(1, items.size());
}


TEST(DataShould, ReturnExistingItemWhenItWasCreatedPreviously)
{

    MockConfiguration config;

    Data data;
    data.m_configuration = &config;
    auto info = data.get(QModelIndex());            //first access - new item
    data.update(info);

    info = data.get(QModelIndex());                 //second access - the same item
    data.update(info);

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


TEST(DataShould, MarkTopItemsAsVisible)
{
    MockQAbstractItemModel model;
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    QModelIndex top = model.createIndex(0, 0, &data);
    data.get(top);                                          //create object
    EXPECT_EQ(true, data.isVisible(QModelIndex()));
}


TEST(DataShould, NotReturnInvisibleItems)
{
    QStandardItemModel model;
    MockConfiguration config;
    const QPixmap pixmap(10, 10);
    const QIcon icon(pixmap);

    Data data;
    data.m_configuration = &config;

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");

    top->appendRow(child1);
    top->appendRow(child2);

    model.appendRow(top);

    //expand top and update items positions
    ModelIndexInfo info = data.get(top->index());
    info.expanded = true;
    data.update(info);

    PositionsCalculator positions_calculator(&model, &data, 100);
    positions_calculator.updateItems();

    auto info1 = data.get(child1->index());
    auto info2 = data.get(child2->index());
    const QRect rect1 = info1.getRect();
    const QRect rect2 = info2.getRect();

    //collapse top
    info.expanded = false;
    data.update(info);

    //even if we ask for point within child area, we should get empty result, as children are invisible
    {
        const QPoint c = rect1.center();
        const ModelIndexInfo& info = data.get(c);

        EXPECT_EQ(QModelIndex(), info.index);
    }
}


TEST(DataShould, NotForgetItemSizeWhenParentCollapsedAndExpanded)
{
    QStandardItemModel model;
    MockConfiguration config;
    const QPixmap pixmap(10, 10);
    const QIcon icon(pixmap);

    Data data;
    data.m_configuration = &config;

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");

    top->appendRow(child1);
    top->appendRow(child2);

    model.appendRow(top);

    //expand top and update items positions
    ModelIndexInfo info = data.get(top->index());
    info.expanded = true;
    data.update(info);

    PositionsCalculator positions_calculator(&model, &data, 100);
    positions_calculator.updateItems();

    auto info1 = data.get(child1->index());
    auto info2 = data.get(child2->index());
    const QRect rect1 = info1.getRect();
    const QRect rect2 = info2.getRect();

    //children size should be calculated
    EXPECT_NE(QRect(), rect1);
    EXPECT_NE(QRect(), rect2);

    //collapse top
    info.expanded = false;
    data.update(info);

    //children size should be preserved
    {
        auto info1 = data.get(child1->index());
        auto info2 = data.get(child2->index());
        EXPECT_EQ(rect1, info1.getRect());
        EXPECT_EQ(rect2, info2.getRect());
    }

    //expand top
    info.expanded = false;
    data.update(info);

    //children size should be preserved
    {
        auto info1 = data.get(child1->index());
        auto info2 = data.get(child2->index());
        EXPECT_EQ(rect1, info1.getRect());
        EXPECT_EQ(rect2, info2.getRect());
    }
}


TEST(DataShould, HideChildrenOfCollapsedNode)
{
    QStandardItemModel model;
    MockConfiguration config;
    const QPixmap pixmap(10, 10);
    const QIcon icon(pixmap);

    Data data;
    data.m_configuration = &config;

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");

    top->appendRow(child1);
    top->appendRow(child2);

    model.appendRow(top);

    //expand top and update items positions
    ModelIndexInfo info = data.get(top->index());
    info.expanded = true;
    data.update(info);

    PositionsCalculator positions_calculator(&model, &data, 100);
    positions_calculator.updateItems();

    //collapse top
    info.expanded = false;
    data.update(info);

    //children should be marked invisible
    {
        EXPECT_EQ(false, data.isVisible(child1->index()));
        EXPECT_EQ(false, data.isVisible(child2->index()));
    }
}
