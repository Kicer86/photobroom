
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <desktop/images_model_view/view_helpers/data.hpp>
#include <desktop/images_model_view/view_helpers/positions_calculator.hpp>

#include "test_helpers/mock_configuration.hpp"
#include "test_helpers/mock_qabstractitemmodel.hpp"

#define SETUP_CONFIG_EXPECTATIONS()                                                     \
    using ::testing::Return;                                                            \
    EXPECT_CALL(config, setDefaultValue(QString("view::margin"), QVariant(2)));         \
    EXPECT_CALL(config, getEntry(QString("view::margin"))).WillRepeatedly(Return(2))

TEST(DataShould, BeConstructable)
{
    MockConfiguration config;
    SETUP_CONFIG_EXPECTATIONS();

    EXPECT_NO_THROW({
        Data data;
        data.set(&config);
    });
}


TEST(DataShould, ContainOnlyRootNodeAfterConstruction)
{
    MockConfiguration config;
    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);

    const auto& items = data.getModel();
    EXPECT_EQ(1, items.size());
}


TEST(DataShould, ContainOnlyRootNodeAfterClear)
{
    MockConfiguration config;
    QStandardItemModel model;

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    const auto& items = data.getModel();
    EXPECT_EQ(1, items.size());
}


TEST(DataShould, ReturnEmptyInfoStructWhenAskedAboutNotExistingItem)
{
    MockConfiguration config;
    QStandardItemModel model;

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    Data::ModelIndexInfoSet::iterator infoIt = data.get(QModelIndex());
    QModelIndex idx = data.get(infoIt);

    EXPECT_EQ(QModelIndex(), idx);

    const auto& items = data.getModel();
    EXPECT_EQ(false, items.empty());
}


TEST(DataShould, SetInitialDataForRootItem)
{
    MockConfiguration config;
    QStandardItemModel model;

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    const ModelIndexInfo& info = *data.get(QModelIndex());
    EXPECT_EQ(true, info.expanded);
    EXPECT_EQ(QRect(), info.getRect());
    EXPECT_EQ(QSize(), info.getOverallSize());
}


TEST(DataShould, StoreInfoAboutItem)
{
    MockConfiguration config;
    QStandardItemModel model;

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    ModelIndexInfo& info = *data.get(QModelIndex());
    info.expanded = true;
    info.setRect(QRect(0, 0, 100, 50));
    info.setOverallSize(QSize(100, 50));

    const ModelIndexInfo& info2 = *data.get(QModelIndex());
    EXPECT_EQ(true, info2.expanded);
    EXPECT_EQ(QRect(0, 0, 100, 50), info2.getRect());
    EXPECT_EQ(QSize(100, 50), info2.getOverallSize());
}


TEST(DataShould, MarkTopItemsAsVisible)
{
    QStandardItemModel model;
    MockConfiguration config;

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    QStandardItem* top = new QStandardItem("Empty");
    model.appendRow(top);

    QModelIndex top_idx = top->index();

    auto top_it = data.get(top_idx);

    EXPECT_EQ(true, top_it.valid());
    EXPECT_EQ(true, data.isVisible(top_it));
}


TEST(DataShould, NotReturnInvisibleItems)
{
    QStandardItemModel model;
    MockConfiguration config;
    const QPixmap pixmap(10, 10);
    const QIcon icon(pixmap);

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&model);
    data.set(&config);

    ViewDataModelObserver mo(&data.getModel(), &model);

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

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

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

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

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
        auto c1_it = data.get(child1->index());
        auto c2_it = data.get(child2->index());

        EXPECT_EQ(true, c1_it.valid());
        EXPECT_EQ(true, c2_it.valid());

        EXPECT_EQ(false, data.isVisible(c1_it));
        EXPECT_EQ(false, data.isVisible(c2_it));
    }
}


TEST(DataShould, ReturnProperIndicesOfItems)
{
    QStandardItemModel model;
    MockConfiguration config;
    const QPixmap pixmap(10, 10);
    const QIcon icon(pixmap);

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

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


TEST(DataShould, ResizeImageAccordinglyToThumbnailHeightHint)
{
    //preparations
    const int img1_w = 200;
    const int img1_h = 100;
    const int img2_w = 100;
    const int img2_h = 500;
    const int canvas_w = 500;

    static MockConfiguration config;
    static QStandardItemModel model;

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
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

    PositionsCalculator calculator(&model, &data, canvas_w);
    calculator.updateItems();

    //// test
    calculator.updateItems();

    // Expectations:
    // We expect both images to get resized to match height = 50px
    {
        const auto child1It = data.get(child1->index());
        const QSize thumb1 = data.getThumbnailSize(child1It);
        const QPixmap pix1 = data.getImage(child1It);
        const QSize pix1Size = pix1.size();

        EXPECT_EQ(QSize(100, 50), thumb1);              // scaled
        EXPECT_EQ(QSize(img1_w, img1_h), pix1Size);     // original

        const auto child2It = data.get(child2->index());
        const QSize thumb2 = data.getThumbnailSize(child2It);
        const QPixmap pix2 = data.getImage(child2It);
        const QSize pix2Size = pix2.size();

        EXPECT_EQ(QSize(10, 50), thumb2);               // scaled
        EXPECT_EQ(QSize(img2_w, img2_h), pix2Size);     // original
    }
}
