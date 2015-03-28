
#include <gtest/gtest.h>

#include <QStandardItemModel>
#include <QIcon>

#include <configuration/constants.hpp>

#include <desktop/model_view/view_helpers/positions_calculator.hpp>
#include <desktop/model_view/view_helpers/positions_reseter.hpp>
#include <desktop/model_view/view_helpers/data.hpp>

#include "test_helpers/mock_configuration.hpp"
#include "test_helpers/mock_qabstractitemmodel.hpp"

TEST(PositionsCalculatorShould, BeConstructable)
{
    EXPECT_NO_THROW({
        MockConfiguration config;
        QStandardItemModel model;

        Data data;
        data.m_configuration = &config;
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

    Data data;
    data.m_configuration = &config;
    data.set(&model);
    
    ViewDataModelObserver mo(&data.getModel(), &model);

    PositionsCalculator calculator(&model, &data, 100);
    calculator.updateItems();

    Data::ModelIndexInfoSet::iterator infoIt = data.get(top);
    const ModelIndexInfo& info = *infoIt;

    EXPECT_EQ(QRect(), info.getRect());
    EXPECT_EQ(QSize(0, 0), info.getOverallSize());
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
    data.m_configuration = &config;
    data.set(&model);
    
    ViewDataModelObserver mo(&data.getModel(), &model);

    PositionsCalculator calculator(&model, &data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info = *data.cfind(QModelIndex());

        EXPECT_EQ(QRect(), info.getRect());                             //invisible
        EXPECT_EQ(QSize(canvas_w, header_h), info.getOverallSize());    //but has overall size of all items
    }

    {
        const ModelIndexInfo& info = *data.cfind(top_idx->index());

        EXPECT_EQ(QRect(0, 0, canvas_w, header_h), info.getRect());     // its position
        EXPECT_EQ(QSize(canvas_w, header_h), info.getOverallSize());    // no children expanded - overall == size
    }
}


TEST(PositionsCalculatorShould, SetMainNodesSizeToCoverItsChild)
{

    // Situation:
    // One node with two children. Node is expanded and its children are visible in one row.

    using ::testing::_;
    using ::testing::Return;
    using ::testing::Invoke;

    const int img_w = 100;
    const int img_h = 50;
    const int margin = 20;
    const int canvas_w = 500;
    const int header_h = 40;

    MockConfiguration config;
    QStandardItemModel model;

    QStandardItem* top_idx = new QStandardItem( "Empty" );
    QStandardItem* top_child1_idx = new QStandardItem( QIcon(QPixmap(img_w, img_h)), "Empty" );

    top_idx->appendRow(top_child1_idx);

    model.appendRow(top_idx);;

    Data view_data;
    view_data.m_configuration = &config;
    view_data.set(&model);
    
    ViewDataModelObserver mo(&view_data.getModel(), &model);

    //expand main node to show children
    ModelIndexInfo& top_info = *view_data.get(top_idx->index());
    top_info.expanded = true;

    PositionsCalculator calculator(&model, &view_data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info = *view_data.cfind(QModelIndex());

        EXPECT_EQ(QRect(), info.getRect());                                                //invisible
        EXPECT_EQ(QSize(canvas_w, header_h + img_h + margin), info.getOverallSize());      //but has overall size of all items
    }

    {
        const ModelIndexInfo& info = *view_data.cfind(top_idx->index());

        EXPECT_EQ(QRect(0, 0, canvas_w, header_h), info.getRect());                        // its position
        EXPECT_EQ(QSize(canvas_w, header_h + img_h + margin), info.getOverallSize());      // no children expanded - overall == size
    }
}


TEST(PositionsCalculatorShould, MoveChildToNextRowIfThereIsNotEnoughtSpace)
{
    const int img_w = 100;
    const int img_h = 50;
    const int margin = 20;
    const int canvas_w = 500;
    const int header_h = 40;

    QStandardItemModel model;
    MockConfiguration config;
    const QPixmap pixmap(img_w, img_h);
    const QIcon icon(pixmap);

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
    view_data.m_configuration = &config;
    view_data.set(&model);
    
    ViewDataModelObserver mo(&view_data.getModel(), &model);

    //expand main node to show children
    ModelIndexInfo& top_info = *view_data.get(top->index());
    top_info.expanded = true;

    PositionsCalculator calculator(&model, &view_data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info = *view_data.cfind(top->index());

        EXPECT_EQ(QRect(0, 0, canvas_w, header_h), info.getRect());                            // its position
        EXPECT_EQ(QSize(canvas_w, header_h + img_h*2 + margin*2), info.getOverallSize());      // we expect two rows
    }

    {
        const ModelIndexInfo& info = *view_data.cfind(child5->index());
        const QRect childSize(0, header_h + img_h + margin, img_w + margin, img_h + margin);  // should start in second row (parent's header + first row height + margin)

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

    Data data;
    data.m_configuration = &config;
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
