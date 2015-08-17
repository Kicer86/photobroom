
#include <gtest/gtest.h>

#include <QStandardItemModel>
#include <QIcon>

#include <configuration/constants.hpp>

#include <desktop/views/view_helpers/positions_calculator.hpp>
#include <desktop/views/view_helpers/positions_reseter.hpp>
#include <desktop/views/view_helpers/data.hpp>

#include "test_helpers/mock_configuration.hpp"
#include "test_helpers/mock_qabstractitemmodel.hpp"

#define SETUP_CONFIG_EXPECTATIONS()                                                     \
    using ::testing::Return;                                                            \
    EXPECT_CALL(config, setDefaultValue(QString("view::spacing"), QVariant(2)));         \
    EXPECT_CALL(config, getEntry(QString("view::spacing"))).WillRepeatedly(Return(2))

TEST(PositionsCalculatorShould, BeConstructable)
{
    EXPECT_NO_THROW({
        MockConfiguration config;
        QStandardItemModel model;

        SETUP_CONFIG_EXPECTATIONS();

        Data data;
        data.set(&config);
        data.set(&model);

        PositionsCalculator calculator(&model, &data, 100);
    });
}


TEST(PositionsCalculatorShould, KeepTopItemSizeEmptyWhenModelIsEmpty)
{
    using ::testing::Return;

    QModelIndex top;

    MockConfiguration config;
    QStandardItemModel model;

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    PositionsCalculator calculator(&model, &data, 100);
    calculator.updateItems();

    Data::ModelIndexInfoSet::iterator infoIt = data.get(top);
    const ModelIndexInfo& info = *infoIt;

    EXPECT_EQ(true, info.isPositionValid());
    EXPECT_EQ(QSize(), info.getSize());
    EXPECT_EQ(QSize(), info.getOverallSize());
}


TEST(PositionsCalculatorShould, SetTopItemsSizeToEmptyEvenIfThereIsAChild)
{
    // Situation:
    // One node with two children. Node is not expanded and is only one visible item.

    using ::testing::_;
    using ::testing::Return;
    using ::testing::Invoke;

    const int canvas_w = 500;
    const int header_h = 40;

    MockConfiguration config;
    QStandardItemModel model;

    SETUP_CONFIG_EXPECTATIONS();

    // top + 1 main node + 2 children
    QStandardItem* top_idx = new QStandardItem;
    QStandardItem* top_child1_idx = new QStandardItem;
    QStandardItem* top_child1_child1_idx = new QStandardItem;
    QStandardItem* top_child1_child2_idx = new QStandardItem;

    top_idx->appendRow(top_child1_idx);
    top_child1_idx->appendRow(top_child1_child1_idx);
    top_child1_idx->appendRow(top_child1_child2_idx);

    model.appendRow(top_idx);

    Data data;
    data.set(&config);
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    PositionsCalculator calculator(&model, &data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info = *data.cfind(QModelIndex());

        EXPECT_EQ(false, info.isSizeValid());                           //invisible
        EXPECT_EQ(QSize(canvas_w, header_h), info.getOverallSize());    //but has overall size of all items
    }

    {
        const ModelIndexInfo& info = *data.cfind(top_idx->index());

        EXPECT_EQ(QRect(0, 0, canvas_w, header_h), info.getRect());     // its position
        EXPECT_EQ(QSize(canvas_w, header_h), info.getOverallSize());    // no children expanded - overall == size
    }
}


TEST(PositionsCalculatorShould, SetMainNodeSizeToCoverItsChild)
{
    // Situation:
    // One node with two children. Node is expanded and its children are visible in one row.

    using ::testing::_;
    using ::testing::Return;
    using ::testing::Invoke;

    MockConfiguration config;
    QStandardItemModel model;

    SETUP_CONFIG_EXPECTATIONS();

    const int img_w = 100;
    const int img_h = 50;

    QStandardItem* top_idx = new QStandardItem( "Empty" );
    QStandardItem* top_child1_idx = new QStandardItem( QIcon(QPixmap(img_w, img_h)), "Empty" );

    top_idx->appendRow(top_child1_idx);

    model.appendRow(top_idx);

    Data view_data;
    view_data.set(&config);
    view_data.set(&model);

    const int spacing = view_data.getSpacing();
    const int canvas_w = 500;
    const int header_h = 40;

    ViewDataModelObserver mo(&view_data.getModel(), &model);

    //expand main node to show children
    ModelIndexInfo& top_info = *view_data.get(top_idx->index());
    top_info.expanded = true;

    PositionsCalculator calculator(&model, &view_data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info = *view_data.cfind(QModelIndex());

        EXPECT_EQ(false, info.isSizeValid());                                               // invisible
        EXPECT_EQ(QSize(canvas_w, header_h + img_h + spacing * 2), info.getOverallSize());  // but has overall size of all items
    }

    {
        const ModelIndexInfo& info = *view_data.cfind(top_idx->index());

        EXPECT_EQ(QRect(0, 0, canvas_w, header_h), info.getRect());                         // its position
        EXPECT_EQ(QSize(canvas_w, header_h + img_h + spacing * 2), info.getOverallSize());  // no children expanded - overall == size
    }
}


TEST(PositionsCalculatorShould, SetMainNodesSizeToCoverItsChildren)
{
    // Situation:
    // two nodes with two children. Second node is expanded and its children are visible in one row.

    using ::testing::_;
    using ::testing::Return;
    using ::testing::Invoke;

    const int img_w = 100;
    const int img_h = 50;

    MockConfiguration config;
    QStandardItemModel model;

    SETUP_CONFIG_EXPECTATIONS();

    QStandardItem* top_idx = new QStandardItem( "Empty" );
    QStandardItem* top_child1_idx = new QStandardItem( QIcon(QPixmap(img_w, img_h)), "Empty" );

    top_idx->appendRow(top_child1_idx);

    QStandardItem* top_idx2 = new QStandardItem( "Empty2" );
    QStandardItem* top2_child1_idx = new QStandardItem( QIcon(QPixmap(img_w, img_h)), "Empty" );

    top_idx2->appendRow(top2_child1_idx);

    model.appendRow(top_idx);
    model.appendRow(top_idx2);

    Data view_data;
    view_data.set(&config);
    view_data.set(&model);

    const int spacing = view_data.getSpacing();
    const int canvas_w = 500;
    const int header_h = 40;

    ViewDataModelObserver mo(&view_data.getModel(), &model);

    //expand second node to show children
    ModelIndexInfo& top_info = *view_data.get(top_idx2->index());
    top_info.expanded = true;

    PositionsCalculator calculator(&model, &view_data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info = *view_data.cfind(top_idx2->index());

        EXPECT_EQ(QRect(0, header_h, canvas_w, header_h), info.getRect());                  // its position - just after first item of height `header_h`
        EXPECT_EQ(QSize(canvas_w, header_h + img_h + spacing * 2), info.getOverallSize());  // no children expanded - overall == size
    }
}


TEST(PositionsCalculatorShould, MoveChildToNextRowIfThereIsNotEnoughtSpace)
{
    const int img_w = 100;
    const int img_h = 50;

    QStandardItemModel model;
    MockConfiguration config;
    const QPixmap pixmap(img_w, img_h);
    const QIcon icon(pixmap);

    SETUP_CONFIG_EXPECTATIONS();

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");
    QStandardItem* child3 = new QStandardItem(icon, "Empty3");
    QStandardItem* child4 = new QStandardItem(icon, "Empty4");
    QStandardItem* child5 = new QStandardItem(icon, "Empty5");

    top->appendRow(child1);
    top->appendRow(child2);
    top->appendRow(child3);
    top->appendRow(child4);
    top->appendRow(child5);

    model.appendRow(top);

    Data view_data;
    view_data.set(&config);
    view_data.set(&model);

    const int spacing = view_data.getSpacing();
    const int canvas_w = 500;
    const int header_h = 40;

    ViewDataModelObserver mo(&view_data.getModel(), &model);

    //expand main node to show children
    ModelIndexInfo& top_info = *view_data.get(top->index());
    top_info.expanded = true;

    PositionsCalculator calculator(&model, &view_data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info = *view_data.cfind(top->index());

        EXPECT_EQ(QRect(0, 0, canvas_w, header_h), info.getRect());                             // its position
        EXPECT_EQ(QSize(canvas_w, header_h + img_h * 2 + spacing * 4), info.getOverallSize());  // we expect two rows
    }

    {
        const ModelIndexInfo& info = *view_data.cfind(child5->index());

        // should start in second row (parent's header + first row height + spacing)
        const QRect childSize(view_data.getImageMargin(),
                              header_h + img_h + spacing * 2, img_w + spacing * 2, img_h + spacing * 2);

        EXPECT_EQ(childSize, info.getRect());
        EXPECT_EQ(childSize.size(), info.getOverallSize());
    }
}


TEST(PositionsCalculatorShould, NotTakeIntoAccountInvisibleItemsWhenCalculatingOverallSize)
{
    //preparations
    const int img_w = 100;
    const int img_h = 50;
    const int canvas_w = 500;

    static MockConfiguration config;
    static QStandardItemModel model;

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    const QPixmap pixmap(img_w, img_h);
    const QIcon icon(pixmap);

    QStandardItem* top = new QStandardItem("Empty");
    QStandardItem* child1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon, "Empty2");
    QStandardItem* child3 = new QStandardItem(icon, "Empty3");
    QStandardItem* child4 = new QStandardItem(icon, "Empty4");
    QStandardItem* child5 = new QStandardItem(icon, "Empty5");

    QStandardItem* top2 = new QStandardItem("Empty");
    QStandardItem* child2_1 = new QStandardItem(icon, "Empty1");
    QStandardItem* child2_2 = new QStandardItem(icon, "Empty2");
    QStandardItem* child2_3 = new QStandardItem(icon, "Empty3");
    QStandardItem* child2_4 = new QStandardItem(icon, "Empty4");
    QStandardItem* child2_5 = new QStandardItem(icon, "Empty5");

    top->appendRow(child1);
    top->appendRow(child2);
    top->appendRow(child3);
    top->appendRow(child4);
    top->appendRow(child5);

    top2->appendRow(child2_1);
    top2->appendRow(child2_2);
    top2->appendRow(child2_3);
    top2->appendRow(child2_4);
    top2->appendRow(child2_5);

    model.appendRow(top);
    model.appendRow(top2);

    //expand main node to show children
    {
        ModelIndexInfo& top_info = *data.get(top->index());
        top_info.expanded = true;

        ModelIndexInfo& top2_info = *data.get(top2->index());
        top2_info.expanded = true;
    }

    PositionsCalculator calculator(&model, &data, canvas_w);
    calculator.updateItems();

    //// test
    ModelIndexInfo& top_info = *data.get(top->index());
    top_info.expanded = false;

    PositionsReseter reseter(&model, &data);
    reseter.itemChanged(top->index());

    calculator.updateItems();

    //expectations
    {
        const ModelIndexInfo& info = *data.cfind(top->index());
        EXPECT_EQ(info.getSize(), info.getOverallSize());       //children are invisible, so both sizes should be equal
    }
}


TEST(PositionsCalculatorShould, FollowDatasThumbnailHeightHint)
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

    const int spacing = data.getSpacing();

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

    // Expectations:
    // We expect both images to get resized to match height = 50px
    {
        const ModelIndexInfo& info1 = *data.cfind(child1->index());
        EXPECT_EQ(QSize(100 + 2 * spacing, 50 + 2 * spacing), info1.getSize());

        const ModelIndexInfo& info2 = *data.cfind(child2->index());
        EXPECT_EQ(QSize(10 + 2 * spacing, 50 + 2 * spacing), info2.getSize());
    }
}


TEST(PositionsCalculatorShould, HandleWideImages)
{
    //preparations
    const int img1_w = 1000;
    const int img1_h = 50;
    const int img2_w = 100;
    const int img2_h = 50;
    const int canvas_w = 500;

    static MockConfiguration config;
    static QStandardItemModel model;

    SETUP_CONFIG_EXPECTATIONS();

    Data data;
    data.set(&config);
    data.set(&model);
    data.setThumbHeight(50);

    const int margin = data.getMargin();

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

    // Expectations:
    // We expect first image to take whole row.
    // Second image should be moved to next one
    {
        const ModelIndexInfo& info1 = *data.cfind(child1->index());
        EXPECT_EQ(QSize(img1_w + 2 * margin, img1_h + 2 * margin), info1.getSize());
        EXPECT_EQ(QPoint(0, 0), info1.getPosition());

        const ModelIndexInfo& info2 = *data.cfind(child2->index());
        EXPECT_EQ(QSize(img2_w + 2 * margin, img2_h + 2 * margin), info2.getSize());
        EXPECT_EQ(QPoint(0, img1_h + 2 * margin), info2.getPosition());
    }
}
