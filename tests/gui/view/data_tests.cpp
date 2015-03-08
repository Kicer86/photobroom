
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <desktop/model_view/view_helpers/data.hpp>
#include <desktop/model_view/view_helpers/positions_calculator.hpp>

#include "test_helpers/mock_configuration.hpp"
#include "test_helpers/mock_qabstractitemmodel.hpp"


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
    QStandardItemModel model;

    Data data;
    data.m_configuration = &config;
    data.set(&model);

    ModelIndexInfoSet::iterator infoIt = data.get(QModelIndex());
    QModelIndex idx = data.get(infoIt);

    EXPECT_EQ(QModelIndex(), idx);

    const auto& items = data.getAll();
    EXPECT_EQ(false, items.empty());
}


TEST(DataShould, StoreDataItemOnUpdate)
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
    data.get(QModelIndex());           //second access - the same item

    const auto& items = data.getAll();
    EXPECT_EQ(1, items.size());
}


TEST(DataShould, ForgetAboutItemWhenAskedForIt)
{
    MockConfiguration config;
    QStandardItemModel model;

    QStandardItem* top = new QStandardItem("Empty");
    model.appendRow(top);

    Data data;
    data.m_configuration = &config;
    data.get(top->index());                      // create new top item + root item (QModelIndex())

    const auto& items = data.getAll();
    EXPECT_EQ(false, items.empty());
    EXPECT_EQ(2, items.size());

    data.forget(top->index());                   // forget about top item

    const auto& itemsAfter = data.getAll();
    EXPECT_EQ(1, itemsAfter.size());             // only root should stay
}


TEST(DataShould, SetInitialDataForRootItem)
{
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    const ModelIndexInfo& info = **data.get(QModelIndex());
    EXPECT_EQ(true, info.expanded);
    EXPECT_EQ(QRect(), info.getRect());
    EXPECT_EQ(QRect(), info.getOverallRect());
}


TEST(DataShould, StoreInfoAboutItem)
{
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    ModelIndexInfo& info = **data.get(QModelIndex());
    info.expanded = true;
    info.setRect(QRect(0, 0, 100, 50));
    info.setOverallRect(QRect(0, 0, 100, 50));

    const ModelIndexInfo& info2 = **data.get(QModelIndex());
    EXPECT_EQ(true, info2.expanded);
    EXPECT_EQ(QRect(0, 0, 100, 50), info2.getRect());
    EXPECT_EQ(QRect(0, 0, 100, 50), info2.getOverallRect());
}


TEST(DataShould, MarkTopItemsAsVisible)
{
    using ::testing::Return;

    MockQAbstractItemModel model;
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    QModelIndex top = model.createIndex(0, 0, &data);

    EXPECT_CALL(model, parent(top)).Times(1).WillRepeatedly(Return(QModelIndex()));

    data.get(top);                          //create object

    EXPECT_EQ(true, data.isVisible(top));
}


TEST(DataShould, NotReturnInvisibleItems)
{
    QStandardItemModel model;
    MockConfiguration config;
    const QPixmap pixmap(10, 10);
    const QIcon icon(pixmap);

    Data data;
    data.set(&model);
    data.m_configuration = &config;

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");

    top->appendRow(child1);
    top->appendRow(child2);

    model.appendRow(top);

    //expand top and update items positions
    ModelIndexInfo& info = **data.get(top->index());
    info.expanded = true;

    PositionsCalculator positions_calculator(&model, &data, 100);
    positions_calculator.updateItems();

    const auto& info1 = **data.get(child1->index());
    const auto& info2 = **data.get(child2->index());
    const QRect rect1 = info1.getRect();
    const QRect rect2 = info2.getRect();

    //collapse top
    info.expanded = false;

    //even if we ask for point within child area, we should get empty result, as children are invisible
    {
        const QPoint c = rect1.center();
        auto infoIt = data.get(c);
        QModelIndex index = data.get(infoIt);

        EXPECT_EQ(QModelIndex(), index);
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
    ModelIndexInfo& info = **data.get(top->index());
    info.expanded = true;

    PositionsCalculator positions_calculator(&model, &data, 100);
    positions_calculator.updateItems();

    const auto& info1 = **data.get(child1->index());
    const auto& info2 = **data.get(child2->index());
    const QRect rect1 = info1.getRect();
    const QRect rect2 = info2.getRect();

    //children size should be calculated
    EXPECT_NE(QRect(), rect1);
    EXPECT_NE(QRect(), rect2);

    //collapse top
    info.expanded = false;

    //children size should be preserved
    {
        const auto& info1 = **data.get(child1->index());
        const auto& info2 = **data.get(child2->index());
        EXPECT_EQ(rect1, info1.getRect());
        EXPECT_EQ(rect2, info2.getRect());
    }

    //expand top
    info.expanded = false;

    //children size should be preserved
    {
        const auto& info1 = **data.get(child1->index());
        const auto& info2 = **data.get(child2->index());
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
    ModelIndexInfo& info = **data.get(top->index());
    info.expanded = true;

    PositionsCalculator positions_calculator(&model, &data, 100);
    positions_calculator.updateItems();

    //collapse top
    info.expanded = false;

    //children should be marked invisible
    {
        EXPECT_EQ(false, data.isVisible(child1->index()));
        EXPECT_EQ(false, data.isVisible(child2->index()));
    }
}
