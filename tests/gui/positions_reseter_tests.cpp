
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <Qt5/model_view/view_helpers/positions_reseter.hpp>
#include <Qt5/model_view/view_helpers/positions_calculator.hpp>
#include <Qt5/model_view/view_helpers/data.hpp>

#include "mock_configuration.hpp"


TEST(PositionsReseterShould, BeConstructable)
{
    EXPECT_NO_THROW({
        MockConfiguration config;

        Data data;
        data.m_configuration = &config;

        PositionsReseter reseter(&data);
    });
}
 

TEST(PositionsReseterShould, ResetProperItemsWhenNewChildIsAdded)
{
    //preparations
    const int img_w = 100;
    const int img_h = 50;
    const int margin = 20;
    const int canvas_w = 500;
    const int header_h = 40;

    static MockConfiguration config;
    static QStandardItemModel model;

    Data data;
    data.m_configuration = &config;

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
        ModelIndexInfo top_info = data.get(top->index());
        top_info.expanded = true;
        data.update(top_info);

        ModelIndexInfo top2_info = data.get(top2->index());
        top2_info.expanded = true;
        data.update(top2_info);
    }

    {
        PositionsCalculator calculator(&model, &data, canvas_w);
        calculator.updateItems();
    }

    //// test
    QStandardItem* child6 = new QStandardItem(icon, "Empty6");
    top->appendRow(child6);

    PositionsReseter reseter(&data);
    reseter.itemsAdded(top->index(), 5);

    //expectations
    {
        ModelIndexInfo info = data.get(top->index());
        EXPECT_NE(info.getRect(), QRect());            // Parent's size should not be reseted
        EXPECT_EQ(info.getOverallRect(), QRect());     // But its overall rect should
    }

    {
        ModelIndexInfo info1 = data.get(child1->index());
        ModelIndexInfo info5 = data.get(child5->index());
        EXPECT_NE(info1.getRect(), QRect());            //siblings should not be reseted
        EXPECT_NE(info5.getRect(), QRect());            //siblings should not be reseted
    }

    {
        ModelIndexInfo info = data.get(top2->index());           //top2's and all its children's positions should be reseted
        ModelIndexInfo info1 = data.get(child2_1->index());
        ModelIndexInfo info5 = data.get(child2_5->index());

        EXPECT_EQ(info.getRect(), QRect());
        EXPECT_EQ(info1.getRect(), QRect());
        EXPECT_EQ(info5.getRect(), QRect());
    }
}



TEST(PositionsReseterShould, ResetProperItemsWhenChildIsRemoved)
{
    //preparations
    const int img_w = 100;
    const int img_h = 50;
    const int margin = 20;
    const int canvas_w = 500;
    const int header_h = 40;

    static MockConfiguration config;
    static QStandardItemModel model;

    Data data;
    data.m_configuration = &config;

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
        ModelIndexInfo top_info = data.get(top->index());
        top_info.expanded = true;
        data.update(top_info);

        ModelIndexInfo top2_info = data.get(top2->index());
        top2_info.expanded = true;
        data.update(top2_info);
    }

    {
        PositionsCalculator calculator(&model, &data, canvas_w);
        calculator.updateItems();
    }

    //// test
    model.removeRow(2, top->index());                  // remove central child of first node (0, 1, 2, 3. 4)

    PositionsReseter reseter(&data);
    reseter.childrenRemoved(top->index(), 2);

    //expectations
    {
        ModelIndexInfo info = data.get(top->index());
        EXPECT_NE(info.getRect(), QRect());            // Parent's size should not be reseted
        EXPECT_EQ(info.getOverallRect(), QRect());     // But its overall rect should
    }

    {
        ModelIndexInfo info1 = data.get(child1->index());
        ModelIndexInfo info2 = data.get(child2->index());
        ModelIndexInfo info4 = data.get(child4->index());
        ModelIndexInfo info5 = data.get(child5->index());
        EXPECT_NE(info1.getRect(), QRect());            //siblings before removed one should not be reseted
        EXPECT_NE(info2.getRect(), QRect());
        EXPECT_EQ(info4.getRect(), QRect());            //siblings after removed one should be reseted
        EXPECT_EQ(info5.getRect(), QRect());
    }

    {
        ModelIndexInfo info = data.get(top2->index());           //top2's and all its children's positions should be reseted
        ModelIndexInfo info1 = data.get(child2_1->index());
        ModelIndexInfo info5 = data.get(child2_5->index());

        EXPECT_EQ(info.getRect(), QRect());
        EXPECT_EQ(info1.getRect(), QRect());
        EXPECT_EQ(info5.getRect(), QRect());
    }
}


TEST(PositionsReseterShould, ResetProperItemsWhenChildChanged)
{
    //preparations
    const int img_w = 100;
    const int img_h = 50;
    const int margin = 20;
    const int canvas_w = 500;
    const int header_h = 40;

    static MockConfiguration config;
    static QStandardItemModel model;

    Data data;
    data.m_configuration = &config;

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
        ModelIndexInfo top_info = data.get(top->index());
        top_info.expanded = true;
        data.update(top_info);

        ModelIndexInfo top2_info = data.get(top2->index());
        top2_info.expanded = true;
        data.update(top2_info);
    }

    {
        PositionsCalculator calculator(&model, &data, canvas_w);
        calculator.updateItems();
    }

    //// test
    PositionsReseter reseter(&data);
    reseter.itemChanged(child3->index());

    //expectations
    {
        ModelIndexInfo info = data.get(top->index());
        EXPECT_NE(info.getRect(), QRect());            // Parent's size should not be reseted
        EXPECT_EQ(info.getOverallRect(), QRect());     // But its overall rect should
    }

    {
        ModelIndexInfo info1 = data.get(child1->index());
        ModelIndexInfo info2 = data.get(child2->index());
        ModelIndexInfo info4 = data.get(child4->index());
        ModelIndexInfo info5 = data.get(child5->index());
        EXPECT_NE(info1.getRect(), QRect());            //siblings before removed one should not be reseted
        EXPECT_NE(info2.getRect(), QRect());
        EXPECT_EQ(info4.getRect(), QRect());            //siblings after removed one should be reseted
        EXPECT_EQ(info5.getRect(), QRect());
    }

    {
        ModelIndexInfo info = data.get(top2->index());           //top2's and all its children's positions should be reseted
        ModelIndexInfo info1 = data.get(child2_1->index());
        ModelIndexInfo info5 = data.get(child2_5->index());

        EXPECT_EQ(info.getRect(), QRect());
        EXPECT_EQ(info1.getRect(), QRect());
        EXPECT_EQ(info5.getRect(), QRect());
    }
}


TEST(PositionsReseterShould, ResetAllItemsWhenAllAreToBeInvalidated)
{
    //preparations
    const int img_w = 100;
    const int img_h = 50;
    const int margin = 20;
    const int canvas_w = 500;
    const int header_h = 40;

    static MockConfiguration config;
    static QStandardItemModel model;

    Data data;
    data.m_configuration = &config;

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
        ModelIndexInfo top_info = data.get(top->index());
        top_info.expanded = true;
        data.update(top_info);

        ModelIndexInfo top2_info = data.get(top2->index());
        top2_info.expanded = true;
        data.update(top2_info);
    }

    {
        PositionsCalculator calculator(&model, &data, canvas_w);
        calculator.updateItems();
    }

    //// test
    PositionsReseter reseter(&data);
    reseter.invalidateAll();

    //expectations
    {
        ModelIndexInfo info = data.get(top->index());
        EXPECT_EQ(info.getRect(), QRect());            // Parent's size should be reseted
        EXPECT_EQ(info.getOverallRect(), QRect());

        info = data.get(top2->index());
        EXPECT_EQ(info.getRect(), QRect());
        EXPECT_EQ(info.getOverallRect(), QRect());
    }

    {
        ModelIndexInfo info1 = data.get(child1->index());
        ModelIndexInfo info5 = data.get(child5->index());
        EXPECT_EQ(info1.getRect(), QRect());
        EXPECT_EQ(info5.getRect(), QRect());
    }

    {
        ModelIndexInfo info1 = data.get(child2_1->index());
        ModelIndexInfo info5 = data.get(child2_5->index());

        EXPECT_EQ(info1.getRect(), QRect());
        EXPECT_EQ(info5.getRect(), QRect());
    }
}


TEST(PositionsReseterShould, ResetProperItemsWhenParentChanged)
{
    //preparations
    const int img_w = 100;
    const int img_h = 50;
    const int margin = 20;
    const int canvas_w = 500;
    const int header_h = 40;

    static MockConfiguration config;
    static QStandardItemModel model;

    Data data;
    data.m_configuration = &config;

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
        ModelIndexInfo top_info = data.get(top->index());
        top_info.expanded = true;
        data.update(top_info);

        ModelIndexInfo top2_info = data.get(top2->index());
        top2_info.expanded = true;
        data.update(top2_info);
    }

    {
        PositionsCalculator calculator(&model, &data, canvas_w);
        calculator.updateItems();
    }

    //// test
    PositionsReseter reseter(&data);
    reseter.itemChanged(child3->index());

    //expectations
    {
        ModelIndexInfo info = data.get(top->index());
        EXPECT_NE(info.getRect(), QRect());            // Parent's size should not be reseted
        EXPECT_EQ(info.getOverallRect(), QRect());     // But its overall rect should
    }

    {
        ModelIndexInfo info1 = data.get(child1->index());
        ModelIndexInfo info2 = data.get(child2->index());
        ModelIndexInfo info4 = data.get(child4->index());
        ModelIndexInfo info5 = data.get(child5->index());
        EXPECT_NE(info1.getRect(), QRect());            //siblings before removed one should not be reseted
        EXPECT_NE(info2.getRect(), QRect());
        EXPECT_EQ(info4.getRect(), QRect());            //siblings after removed one should be reseted
        EXPECT_EQ(info5.getRect(), QRect());
    }

    {
        ModelIndexInfo info = data.get(top2->index());           //top2's and all its children's positions should be reseted
        ModelIndexInfo info1 = data.get(child2_1->index());
        ModelIndexInfo info5 = data.get(child2_5->index());

        EXPECT_EQ(info.getRect(), QRect());
        EXPECT_EQ(info1.getRect(), QRect());
        EXPECT_EQ(info5.getRect(), QRect());
    }
}
