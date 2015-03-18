
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
 

TEST(DataShould, ContainOnlyRootNodeAfterConstruction)
{

    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    const auto& items = data.getAll();
    EXPECT_EQ(0, items.size());
}


TEST(DataShould, ContainOnlyRootNodeAfterClear)
{

    MockConfiguration config;

    Data data;
    data.m_configuration = &config;

    const auto& items = data.getAll();
    EXPECT_EQ(1, items.size());
}


TEST(DataShould, ReturnEmptyInfoStructWhenAskedAboutNotExistingItem)
{
    MockConfiguration config;
    QStandardItemModel model;

    Data data;
    data.m_configuration = &config;
    data.set(&model);

    Data::ModelIndexInfoSet::iterator infoIt = data.get(QModelIndex());
    QModelIndex idx = data.get(infoIt);

    EXPECT_EQ(QModelIndex(), idx);

    const auto& items = data.getAll();
    EXPECT_EQ(false, items.empty());
}


TEST(DataShould, SetInitialDataForRootItem)
{
    MockConfiguration config;
    QStandardItemModel model;

    Data data;
    data.m_configuration = &config;
    data.set(&model);

    const ModelIndexInfo& info = *data.get(QModelIndex());
    EXPECT_EQ(true, info.expanded);
    EXPECT_EQ(QRect(), info.getRect());
    EXPECT_EQ(QRect(), info.getOverallRect());
}


TEST(DataShould, StoreInfoAboutItem)
{
    MockConfiguration config;
    QStandardItemModel model;

    Data data;
    data.m_configuration = &config;
    data.set(&model);

    ModelIndexInfo& info = *data.get(QModelIndex());
    info.expanded = true;
    info.setRect(QRect(0, 0, 100, 50));
    info.setOverallRect(QRect(0, 0, 100, 50));

    const ModelIndexInfo& info2 = *data.get(QModelIndex());
    EXPECT_EQ(true, info2.expanded);
    EXPECT_EQ(QRect(0, 0, 100, 50), info2.getRect());
    EXPECT_EQ(QRect(0, 0, 100, 50), info2.getOverallRect());
}


TEST(DataShould, MarkTopItemsAsVisible)
{
    using ::testing::Return;

    QStandardItemModel model;
    MockConfiguration config;

    Data data;
    data.m_configuration = &config;
    data.set(&model);

    QStandardItem* top = new QStandardItem("Empty");
    model.appendRow(top);

    QModelIndex top_idx = top->index();

    data.get(top_idx);

    EXPECT_EQ(true, data.isVisible(top_idx));
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
    ModelIndexInfo& info = *data.get(top->index());
    info.expanded = true;

    PositionsCalculator positions_calculator(&model, &data, 100);
    positions_calculator.updateItems();

    const auto& info1 = *data.get(child1->index());
    const auto& info2 = *data.get(child2->index());
    const QRect rect1 = info1.getRect();
    const QRect rect2 = info2.getRect();

    //collapse top
    info.expanded = false;

    //even if we ask for point within child area, we should get empty result, as children are invisible
    {
        const QPoint c = rect1.center();
        auto infoIt = data.get(c);
        const QModelIndex index = data.get(infoIt);

        EXPECT_EQ(QModelIndex(), index);
        
        const QPoint c2 = rect2.center();
        auto infoIt2 = data.get(c2);
        const QModelIndex index2 = data.get(infoIt2);

        EXPECT_EQ(QModelIndex(), index2);
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
    data.set(&model);

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");

    top->appendRow(child1);
    top->appendRow(child2);

    model.appendRow(top);

    //expand top and update items positions
    ModelIndexInfo& info = *data.get(top->index());
    info.expanded = true;

    PositionsCalculator positions_calculator(&model, &data, 100);
    positions_calculator.updateItems();

    const auto& f_info1 = *data.get(child1->index());
    const auto& f_info2 = *data.get(child2->index());
    const QRect rect1 = f_info1.getRect();
    const QRect rect2 = f_info2.getRect();

    //children size should be calculated
    EXPECT_NE(QRect(), rect1);
    EXPECT_NE(QRect(), rect2);

    //collapse top
    info.expanded = false;

    //children size should be preserved
    {
        const auto& info1 = *data.get(child1->index());
        const auto& info2 = *data.get(child2->index());
        EXPECT_EQ(rect1, info1.getRect());
        EXPECT_EQ(rect2, info2.getRect());
    }

    //expand top
    info.expanded = false;

    //children size should be preserved
    {
        const auto& info1 = *data.get(child1->index());
        const auto& info2 = *data.get(child2->index());
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
    data.set(&model);

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");

    top->appendRow(child1);
    top->appendRow(child2);

    model.appendRow(top);

    //expand top and update items positions
    ModelIndexInfo& info = *data.get(top->index());
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


TEST(DataShould, ReturnProperIndicesOfItems)
{
    QStandardItemModel model;
    MockConfiguration config;
    const QPixmap pixmap(10, 10);
    const QIcon icon(pixmap);

    Data data;
    data.m_configuration = &config;
    data.set(&model);

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");

    top->appendRow(child1);
    top->appendRow(child2);

    model.appendRow(top);

    //expand top so children will be stored in 'data' when calculating positions
    auto it = data.get(top->index());
    it->expanded = true;

    PositionsCalculator positions_calculator(&model, &data, 100);
    positions_calculator.updateItems();

    {
        auto it1 = data.get(top->index());
        QModelIndex topIdx = data.get(it1);

        auto it2 = data.get(child1->index());
        QModelIndex child1Idx = data.get(it2);

        auto it3 = data.get(child2->index());
        QModelIndex child2Idx = data.get(it3);

        EXPECT_EQ(top->index(), topIdx);
        EXPECT_EQ(child1->index(), child1Idx);
        EXPECT_EQ(child2->index(), child2Idx);
    }
}
