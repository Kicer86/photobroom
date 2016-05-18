
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <desktop/views/view_impl/data.hpp>
#include <desktop/views/view_impl/positions_calculator.hpp>
#include <desktop/views/view_impl/positions_translator.hpp>

#include "test_helpers/mock_configuration.hpp"
#include "test_helpers/mock_qabstractitemmodel.hpp"


TEST(DataShould, BeConstructable)
{
    EXPECT_NO_THROW({
        Data data;
    });
}


TEST(DataShould, ContainOnlyRootNodeAfterConstruction)
{
    Data data;

    const auto& items = data.getModel();
    EXPECT_EQ(1, items.size());
}


TEST(DataShould, ContainOnlyRootNodeAfterClear)
{
    QStandardItemModel model;

    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    const auto& items = data.getModel();
    EXPECT_EQ(1, items.size());
}


TEST(DataShould, SetInitialDataForRootItem)
{
    QStandardItemModel model;

    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    const ModelIndexInfo& info = data.get(QModelIndex())->second;
    EXPECT_EQ(true, info.expanded);
    EXPECT_EQ(false, info.isPositionValid());
    EXPECT_EQ(false, info.isSizeValid());
    EXPECT_EQ(false, info.isOverallSizeValid());
}


TEST(DataShould, StoreInfoAboutItem)
{
    QStandardItemModel model;

    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    ModelIndexInfo& info = data.get(QModelIndex())->second;
    info.expanded = true;
    info.setRect(QRect(0, 0, 100, 50));
    info.setOverallSize(QSize(100, 50));

    const ModelIndexInfo& info2 = data.get(QModelIndex())->second;
    EXPECT_EQ(true, info2.expanded);
    EXPECT_EQ(QRect(0, 0, 100, 50), info2.getRect());
    EXPECT_EQ(QSize(100, 50), info2.getOverallSize());
}


TEST(DataShould, MarkTopItemsAsVisible)
{
    QStandardItemModel model;

    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    QStandardItem* top = new QStandardItem("Empty");
    model.appendRow(top);

    QModelIndex top_idx = top->index();

    auto top_it = data.get(top_idx);

    EXPECT_EQ(true, data.isValid(top_it));
    EXPECT_EQ(true, data.isVisible(top_idx));
}


TEST(DataShould, NotReturnInvisibleItems)
{
    QStandardItemModel model;
    const QPixmap pixmap(10, 10);
    const QIcon icon(pixmap);

    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");

    top->appendRow(child1);
    top->appendRow(child2);

    model.appendRow(top);

    //expand top and update items positions
    ModelIndexInfo& info = data.get(top->index())->second;
    info.expanded = true;

    PositionsTranslator translator(&data);

    PositionsCalculator positions_calculator(&data, 100);
    positions_calculator.updateItems();

    const QRect rect1 = translator.getAbsoluteRect(child1->index());
    const QRect rect2 = translator.getAbsoluteRect(child2->index());

    //collapse top
    info.expanded = false;

    //even if we ask for point within child area, we should get empty result, as children are invisible
    {
        const QPoint c = rect1.center();
        const QModelIndex index = data.get(c);

        EXPECT_EQ(QModelIndex(), index);

        const QPoint c2 = rect2.center();
        const QModelIndex index2 = data.get(c2);

        EXPECT_EQ(QModelIndex(), index2);
    }
}


TEST(DataShould, NotForgetItemSizeWhenParentCollapsedAndExpanded)
{
    QStandardItemModel model;
    const QPixmap pixmap(10, 10);
    const QIcon icon(pixmap);

    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");

    top->appendRow(child1);
    top->appendRow(child2);

    model.appendRow(top);

    //expand top and update items positions
    ModelIndexInfo& info = data.get(top->index())->second;
    info.expanded = true;

    PositionsCalculator positions_calculator(&data, 100);
    positions_calculator.updateItems();

    const auto& f_info1 = data.get(child1->index())->second;
    const auto& f_info2 = data.get(child2->index())->second;
    const QRect rect1 = f_info1.getRect();
    const QRect rect2 = f_info2.getRect();

    //children size should be calculated
    EXPECT_NE(QRect(), rect1);
    EXPECT_NE(QRect(), rect2);

    //collapse top
    info.expanded = false;

    //children size should be preserved
    {
        const auto& info1 = data.get(child1->index())->second;
        const auto& info2 = data.get(child2->index())->second;
        EXPECT_EQ(rect1, info1.getRect());
        EXPECT_EQ(rect2, info2.getRect());
    }

    //expand top
    info.expanded = false;

    //children size should be preserved
    {
        const auto& info1 = data.get(child1->index())->second;
        const auto& info2 = data.get(child2->index())->second;
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
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");

    top->appendRow(child1);
    top->appendRow(child2);

    model.appendRow(top);

    //expand top and update items positions
    ModelIndexInfo& info = data.get(top->index())->second;
    info.expanded = true;

    PositionsCalculator positions_calculator(&data, 100);
    positions_calculator.updateItems();

    //collapse top
    info.expanded = false;

    //children should be marked invisible
    {
        auto c1_it = data.get(child1->index());
        auto c2_it = data.get(child2->index());

        EXPECT_EQ(true, data.isValid(c1_it));
        EXPECT_EQ(true, data.isValid(c2_it));

        EXPECT_EQ(false, data.isVisible(child1->index()));
        EXPECT_EQ(false, data.isVisible(child2->index()));
    }
}


TEST(DataShould, ResizeImageAccordinglyToThumbnailHeightHint)
{
    //preparations
    const int img1_w = 200;
    const int img1_h = 100;
    const int img2_w = 100;
    const int img2_h = 500;
    const int canvas_w = 500;

    QStandardItemModel model;

    Data data;
    data.set(&model);
    data.setThumbHeight(50);

    ViewDataModelObserver mo(&data.getModel(), &model);

    const QPixmap pixmap1(img1_w, img1_h);
    const QIcon icon1(pixmap1);

    const QPixmap pixmap2(img2_w, img2_h);
    const QIcon icon2(pixmap2);

    QStandardItem* child1 = new QStandardItem(icon1, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon2, "Empty2");

    model.appendRow(child1);
    model.appendRow(child2);

    PositionsCalculator calculator(&data, canvas_w);
    calculator.updateItems();

    //// test
    calculator.updateItems();

    // Expectations:
    // We expect both images to get resized to match height = 50px
    {
        const QSize thumb1 = data.getThumbnailSize(child1->index());
        const QPixmap pix1 = data.getImage(child1->index());
        const QSize pix1Size = pix1.size();

        EXPECT_EQ(QSize(100, 50), thumb1);              // scaled
        EXPECT_EQ(QSize(img1_w, img1_h), pix1Size);     // original

        const QSize thumb2 = data.getThumbnailSize(child2->index());
        const QPixmap pix2 = data.getImage(child2->index());
        const QSize pix2Size = pix2.size();

        EXPECT_EQ(QSize(10, 50), thumb2);               // scaled
        EXPECT_EQ(QSize(img2_w, img2_h), pix2Size);     // original
    }
}
