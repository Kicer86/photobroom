
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <desktop/model_view/view_helpers/positions_reseter.hpp>
#include <desktop/model_view/view_helpers/positions_calculator.hpp>
#include <desktop/model_view/view_helpers/data.hpp>

#include "test_helpers/mock_configuration.hpp"


class PositionsReseterShould : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        QPixmap pixmap(img_w, img_h);
        icon = pixmap;

        data.m_configuration = &config;

        top = new QStandardItem("Empty");
        child1 = new QStandardItem(icon, "Empty1");
        child2 = new QStandardItem(icon, "Empty2");
        child3 = new QStandardItem(icon, "Empty3");
        child4 = new QStandardItem(icon, "Empty4");
        child5 = new QStandardItem(icon, "Empty5");

        top2 = new QStandardItem("Empty");
        child2_1 = new QStandardItem(icon, "Empty1");
        child2_2 = new QStandardItem(icon, "Empty2");
        child2_3 = new QStandardItem(icon, "Empty3");
        child2_4 = new QStandardItem(icon, "Empty4");
        child2_5 = new QStandardItem(icon, "Empty5");

        top3 = new QStandardItem("Empty");
        child3_1 = new QStandardItem(icon, "Empty1");
        child3_2 = new QStandardItem(icon, "Empty2");
        child3_3 = new QStandardItem(icon, "Empty3");
        child3_4 = new QStandardItem(icon, "Empty4");
        child3_5 = new QStandardItem(icon, "Empty5");

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

        top3->appendRow(child3_1);
        top3->appendRow(child3_2);
        top3->appendRow(child3_3);
        top3->appendRow(child3_4);
        top3->appendRow(child3_5);

        model.appendRow(top);
        model.appendRow(top2);
        model.appendRow(top3);
    }

    const int img_w = 100;
    const int img_h = 50;
    const int canvas_w = 500;

    QIcon icon;

    Data data;
    MockConfiguration config;
    QStandardItemModel model;

    QStandardItem* top;
    QStandardItem* child1;
    QStandardItem* child2;
    QStandardItem* child3;
    QStandardItem* child4;
    QStandardItem* child5;

    QStandardItem* top2;
    QStandardItem* child2_1;
    QStandardItem* child2_2;
    QStandardItem* child2_3;
    QStandardItem* child2_4;
    QStandardItem* child2_5;

    QStandardItem* top3;
    QStandardItem* child3_1;
    QStandardItem* child3_2;
    QStandardItem* child3_3;
    QStandardItem* child3_4;
    QStandardItem* child3_5;
};


TEST_F(PositionsReseterShould, BeConstructable)
{
    EXPECT_NO_THROW({
        PositionsReseter reseter(&model, &data);
    });
}
 

TEST_F(PositionsReseterShould, ResetProperItemsWhenNewChildIsAdded)
{
    //expand main node to show children
    {
        ModelIndexInfo top_info = data.get(top->index());
        top_info.expanded = true;
        data.update(top_info);

        ModelIndexInfo top2_info = data.get(top2->index());
        top2_info.expanded = true;
        data.update(top2_info);

        PositionsCalculator calculator(&model, &data, canvas_w);
        calculator.updateItems();
    }

    //// test
    QStandardItem* child6 = new QStandardItem(icon, "Empty6");
    top->appendRow(child6);

    PositionsReseter reseter(&model, &data);
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


TEST_F(PositionsReseterShould, ResetProperItemsWhenChildIsRemoved)
{
    //expand main node to show children
    {
        ModelIndexInfo top_info = data.get(top->index());
        top_info.expanded = true;
        data.update(top_info);

        ModelIndexInfo top2_info = data.get(top2->index());
        top2_info.expanded = true;
        data.update(top2_info);

        PositionsCalculator calculator(&model, &data, canvas_w);
        calculator.updateItems();
    }

    //// test
    model.removeRow(2, top->index());                  // remove central child of first node (0, 1, 2, 3, 4)

    PositionsReseter reseter(&model, &data);
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


TEST_F(PositionsReseterShould, ResetProperItemsWhenChildChanged)
{
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
    PositionsReseter reseter(&model, &data);
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


TEST_F(PositionsReseterShould, ResetProperItemsWhenNodeChanges)
{
    //expand main nodes to show children
    {
        ModelIndexInfo top_info = data.get(top->index());
        top_info.expanded = true;
        data.update(top_info);

        ModelIndexInfo top2_info = data.get(top2->index());
        top2_info.expanded = true;
        data.update(top2_info);

        PositionsCalculator calculator(&model, &data, canvas_w);
        calculator.updateItems();
    }

    //// test
    PositionsReseter reseter(&model, &data);
    reseter.itemChanged(top->index());

    //expectations
    {
        ModelIndexInfo info = data.get(top->index());
        EXPECT_EQ(QRect(), info.getRect());            // Node's size should be reseted.
        EXPECT_EQ(QRect(), info.getOverallRect());     // So overall size should.
    }


    {
        //children should not be reseted
        ModelIndexInfo info1 = data.get(child1->index());
        ModelIndexInfo info5 = data.get(child5->index());
        EXPECT_NE(QRect(), info1.getRect());
        EXPECT_NE(QRect(), info5.getRect());
    }

    {
        ModelIndexInfo info = data.get(top2->index());           //top2's and all its children's positions should be reseted
        ModelIndexInfo info1 = data.get(child2_1->index());
        ModelIndexInfo info5 = data.get(child2_5->index());

        EXPECT_EQ(QRect(), info.getRect());
        EXPECT_EQ(QRect(), info1.getRect());
        EXPECT_EQ(QRect(), info5.getRect());
    }
}


TEST_F(PositionsReseterShould, ResetAllItemsWhenAllAreToBeInvalidated)
{
    //expand main node to show children
    {
        ModelIndexInfo top_info = data.get(top->index());
        top_info.expanded = true;
        data.update(top_info);

        ModelIndexInfo top2_info = data.get(top2->index());
        top2_info.expanded = true;
        data.update(top2_info);

        PositionsCalculator calculator(&model, &data, canvas_w);
        calculator.updateItems();
    }

    //// test
    PositionsReseter reseter(&model, &data);
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


TEST_F(PositionsReseterShould, ResetProperItemsWhenParentChanged)
{
    //expand main node to show children
    {
        ModelIndexInfo top_info = data.get(top->index());
        top_info.expanded = true;
        data.update(top_info);

        ModelIndexInfo top2_info = data.get(top2->index());
        top2_info.expanded = true;
        data.update(top2_info);

        PositionsCalculator calculator(&model, &data, canvas_w);
        calculator.updateItems();
    }

    //// test
    PositionsReseter reseter(&model, &data);
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


TEST_F(PositionsReseterShould, ResetSiblingsWhenItemRemoved)
{
    //expand main node to show children
    {
        ModelIndexInfo top_info = data.get(top->index());
        top_info.expanded = true;
        data.update(top_info);

        ModelIndexInfo top2_info = data.get(top2->index());
        top2_info.expanded = true;
        data.update(top2_info);

        PositionsCalculator calculator(&model, &data, canvas_w);
        calculator.updateItems();
    }

    // test
    model.removeRow(0, top->index());

    PositionsReseter reseter(&model, &data);
    reseter.childrenRemoved(top->index(), 0);

    //expectations
    {
        ModelIndexInfo info = data.get(top->index());
        EXPECT_NE(info.getRect(), QRect());            // Parent's size should not be reseted
        EXPECT_EQ(info.getOverallRect(), QRect());     // But its overall rect should
    }

    {
        ModelIndexInfo info2 = data.get(child2->index());
        ModelIndexInfo info3 = data.get(child3->index());
        ModelIndexInfo info4 = data.get(child4->index());
        ModelIndexInfo info5 = data.get(child5->index());
        EXPECT_EQ(info2.getRect(), QRect());            //siblings after removed one should be reseted
        EXPECT_EQ(info3.getRect(), QRect());
        EXPECT_EQ(info4.getRect(), QRect());
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


TEST_F(PositionsReseterShould, NotResetParentOrItsSiblignsWhenParentIsCollapsedAndChildChanges)
{
    //prepare data
    PositionsCalculator calculator(&model, &data, canvas_w);
    calculator.updateItems();
    
    // test
    model.removeRow(0, top->index());

    PositionsReseter reseter(&model, &data);
    reseter.childrenRemoved(top->index(), 0);

   //expectations
    {
        ModelIndexInfo info = data.get(top->index());
        EXPECT_NE(info.getRect(), QRect());               // Parent's size should not be reseted
        EXPECT_NE(info.getOverallRect(), QRect());
    }

    {
        ModelIndexInfo info2 = data.get(top2->index());   // Parent's siblings should not be touched
        EXPECT_NE(QRect(), info2.getRect());
        EXPECT_NE(QRect(), info2.getOverallRect());
    }
}


TEST_F(PositionsReseterShould, InvalidateProperTopItemsWhenNewOneAppear)
{
    //prepare data
    PositionsCalculator calculator(&model, &data, canvas_w);
    calculator.updateItems();

    // test
    QStandardItem* new_top0 = new QStandardItem("Empty");
    model.insertRow(0, new_top0);

    PositionsReseter reseter(&model, &data);
    reseter.itemsAdded(QModelIndex(), 0);

    //expectations
    {
        //all top items should be reseted
        ModelIndexInfo info = data.get(top->index());
        EXPECT_EQ(QRect(), info.getRect());
        EXPECT_EQ(QRect(), info.getOverallRect());

        ModelIndexInfo info2 = data.get(top2->index());
        EXPECT_EQ(QRect(), info2.getRect());
        EXPECT_EQ(QRect(), info2.getOverallRect());

        ModelIndexInfo info3 = data.get(top3->index());
        EXPECT_EQ(QRect(), info3.getRect());
        EXPECT_EQ(QRect(), info3.getOverallRect());
    }
}


TEST_F(PositionsReseterShould, InvalidateProperTopItemsWhenOneOfTopItemsIsBeingRemoved)
{
    //prepare data
    PositionsCalculator calculator(&model, &data, canvas_w);
    calculator.updateItems();

    // test
    model.removeRow(1);

    PositionsReseter reseter(&model, &data);
    reseter.childrenRemoved(QModelIndex(), 1);

    //expectations
    {
        //all top items should be reseted
        ModelIndexInfo info = data.get(top->index());
        EXPECT_NE(QRect(), info.getRect());
        EXPECT_NE(QRect(), info.getOverallRect());

        ModelIndexInfo info3 = data.get(top3->index());
        EXPECT_EQ(QRect(), info3.getRect());
        EXPECT_EQ(QRect(), info3.getOverallRect());
    }
}
