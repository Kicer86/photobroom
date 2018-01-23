
#include <gtest/gtest.h>

#include <QIcon>
#include <QStandardItemModel>

#include <core/constants.hpp>

#include <desktop/views/view_impl/positions_calculator.hpp>
#include <desktop/views/view_impl/positions_reseter.hpp>
#include <desktop/views/view_impl/data.hpp>

#include "unit_tests_utils/mock_photo_info.hpp"
#include "test_helpers/mock_configuration.hpp"
#include "test_helpers/mock_qabstractitemmodel.hpp"
#include "test_helpers/photo_info_model.hpp"


using ::testing::_;
using ::testing::ReturnRef;
using ::testing::Return;


class PositionsCalculatorShould: public ::testing::Test
{
    public:
        PositionsCalculatorShould():
            testing::Test(),
            submodel(),
            model(&submodel)
        {
        }

    protected:
        virtual void SetUp() override
        {

        }

        QStandardItemModel submodel;
        MockPhotoInfoModel model;
};

TEST_F(PositionsCalculatorShould, BeConstructable)
{
    EXPECT_NO_THROW({
        Data data;
        data.set(&model);

        PositionsCalculator calculator(&data, 100);
    });
}


TEST_F(PositionsCalculatorShould, KeepTopItemSizeEmptyWhenModelIsEmpty)
{
    QModelIndex top;

    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    PositionsCalculator calculator(&data, 100);
    calculator.updateItems();

    const ModelIndexInfo& info = data.get(top);

    EXPECT_EQ(true, info.isPositionValid());
    EXPECT_EQ(QSize(0, 0), info.getSize());
    EXPECT_EQ(QSize(0, 0), info.getOverallSize());
}


TEST_F(PositionsCalculatorShould, SetTopItemsSizeToEmptyEvenIfThereIsAChild)
{
    // Situation:
    // One node with two children. Node is not expanded and is only one visible item.

    const int canvas_w = 500;
    const int header_h = 40;

    // top + 1 main node + 2 children
    QStandardItem* top_idx = new QStandardItem;
    QStandardItem* top_child1_idx = new QStandardItem;
    QStandardItem* top_child1_child1_idx = new QStandardItem;
    QStandardItem* top_child1_child2_idx = new QStandardItem;

    top_idx->appendRow(top_child1_idx);
    top_child1_idx->appendRow(top_child1_child1_idx);
    top_child1_idx->appendRow(top_child1_child2_idx);

    submodel.appendRow(top_idx);

    Data data;
    data.set(&model);

    const int margin  = data.getImageMargin();

    ViewDataModelObserver mo(&data.getModel(), &model);

    PositionsCalculator calculator(&data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info = *data.cfind(QModelIndex());

        EXPECT_EQ(false, info.isSizeValid());                                    //invisible
        EXPECT_EQ(QSize(canvas_w, header_h + margin), info.getOverallSize());    //but has overall size of all items
    }

    {
        const ModelIndexInfo& info = *data.cfind(top_idx->index());

        EXPECT_EQ(QRect(0, 0, canvas_w, header_h), info.getRect());     // its position
        EXPECT_EQ(QSize(canvas_w, header_h), info.getOverallSize());    // no children expanded - overall == size
    }
}


TEST_F(PositionsCalculatorShould, SetMainNodeSizeToCoverItsChild)
{
    // Situation:
    // One node with two children. Node is expanded and its children are visible in one row.

    using ::testing::_;
    using ::testing::Return;

    const int img_w = 100;
    const int img_h = 50;

    QStandardItem* top_idx = new QStandardItem( "Empty" );
    QStandardItem* top_child1_idx = new QStandardItem( QIcon(QPixmap(img_w, img_h)), "Empty" );

    top_idx->appendRow(top_child1_idx);

    submodel.appendRow(top_idx);

    Data view_data;
    view_data.set(&model);

    const int spacing = view_data.getSpacing();
    const int margin  = view_data.getImageMargin();
    const int canvas_w = 500;
    const int header_h = 40;
    const int thumb_h = view_data.getThumbnailDesiredHeight();

    ViewDataModelObserver mo(&view_data.getModel(), &model);

    // setup expectations
    Photo::Data photoDetails;
    photoDetails.geometry = QSize(100, 50);
    photoDetails.id = 0;
    photoDetails.path = "";

    EXPECT_CALL(model, getPhotoDetails(_)).WillRepeatedly(ReturnRef(photoDetails));
    //

    //expand main node to show children
    ModelIndexInfo& top_info = view_data.get(top_idx->index());
    top_info.expanded = true;

    PositionsCalculator calculator(&view_data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info = *view_data.cfind(QModelIndex());

        EXPECT_EQ(false, info.isSizeValid());                                                            // invisible
        EXPECT_EQ(QSize(canvas_w, header_h + thumb_h + spacing * 2 + margin * 2), info.getOverallSize());  // but has overall size of all items
    }

    {
        const ModelIndexInfo& info = *view_data.cfind(top_idx->index());

        EXPECT_EQ(QRect(0, 0, canvas_w, header_h), info.getRect());                                  // its position
        EXPECT_EQ(QSize(canvas_w, header_h + thumb_h + spacing * 2 + margin), info.getOverallSize());  // no children expanded - overall == size
    }
}


TEST_F(PositionsCalculatorShould, SetMainNodesSizeToCoverItsChildren)
{
    // Situation:
    // two nodes with two children. Second node is expanded and its children are visible in one row.

    const QSize img(100, 50);

    QStandardItem* top_idx = new QStandardItem( "Empty" );
    QStandardItem* top_child1_idx = new QStandardItem( QIcon(QPixmap(img)), "Empty" );

    top_idx->appendRow(top_child1_idx);

    QStandardItem* top_idx2 = new QStandardItem( "Empty2" );
    QStandardItem* top2_child1_idx = new QStandardItem( QIcon(QPixmap(img)), "Empty" );

    top_idx2->appendRow(top2_child1_idx);

    submodel.appendRow(top_idx);
    submodel.appendRow(top_idx2);

    Data view_data;
    view_data.set(&model);

    const int spacing = view_data.getSpacing();
    const int margin  = view_data.getImageMargin();
    const int canvas_w = 500;
    const int header_h = 40;
    const int thumb_h = view_data.getThumbnailDesiredHeight();

    // setup expectations
    Photo::Data photoDetails;
    photoDetails.geometry = img;
    photoDetails.id = 0;
    photoDetails.path = "";

    EXPECT_CALL(model, getPhotoDetails(_)).WillRepeatedly(ReturnRef(photoDetails));
    //

    ViewDataModelObserver mo(&view_data.getModel(), &model);

    //expand second node to show children
    ModelIndexInfo& top_info2 = view_data.get(top_idx2->index());
    top_info2.expanded = true;

    PositionsCalculator calculator(&view_data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info2 = *view_data.cfind(top_idx2->index());

        EXPECT_EQ(QRect(0, header_h, canvas_w, header_h), info2.getRect());                           // its position - just after first item of height `header_h`
        EXPECT_EQ(QSize(canvas_w, header_h + thumb_h + spacing * 2 + margin), info2.getOverallSize());  // no children expanded - overall == size
    }
}


TEST_F(PositionsCalculatorShould, MoveChildToNextRowIfThereIsNotEnoughtSpace)
{
    const QSize img(100, 50);

    const QPixmap pixmap(img);
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

    submodel.appendRow(top);

    Data view_data;
    view_data.set(&model);
    view_data.setThumbnailDesiredHeight(img.height());

    const int spacing = view_data.getSpacing();
    const int margin  = view_data.getImageMargin();
    const int canvas_w = 500;
    const int header_h = 40;

    // setup expectations
    Photo::Data photoDetails;
    photoDetails.geometry = img;
    photoDetails.id = 0;
    photoDetails.path = "";

    EXPECT_CALL(model, getPhotoDetails(_)).WillRepeatedly(ReturnRef(photoDetails));
    //

    ViewDataModelObserver mo(&view_data.getModel(), &model);

    //expand main node to show children
    ModelIndexInfo& top_info = view_data.get(top->index());
    top_info.expanded = true;

    PositionsCalculator calculator(&view_data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info = *view_data.cfind(top->index());

        EXPECT_EQ(QRect(0, 0, canvas_w, header_h), info.getRect());                                        // its position
        EXPECT_EQ(QSize(canvas_w, header_h + img.height() * 2 + spacing * 4 + margin), info.getOverallSize());  // we expect two rows
    }

    {
        const ModelIndexInfo& info = *view_data.cfind(child5->index());

        // should start in second row (first row height + spacing)
        const QRect childRect(view_data.getImageMargin(),
                              img.height() + spacing * 2, img.width() + spacing * 2, img.height() + spacing * 2);

        EXPECT_EQ(childRect, info.getRect());
        EXPECT_EQ(childRect.size(), info.getOverallSize());
    }
}


TEST_F(PositionsCalculatorShould, NotTakeIntoAccountInvisibleItemsWhenCalculatingOverallSize)
{
    //preparations
    const QSize img(100, 50);
    const int canvas_w = 500;

    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    const QPixmap pixmap(img);
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

    submodel.appendRow(top);
    submodel.appendRow(top2);

    // setup expectations
    Photo::Data photoDetails;
    photoDetails.geometry = img;
    photoDetails.id = 0;
    photoDetails.path = "";

    EXPECT_CALL(model, getPhotoDetails(_)).WillRepeatedly(ReturnRef(photoDetails));
    //

    //expand main node to show children
    {
        ModelIndexInfo& top_info = data.get(top->index());
        top_info.expanded = true;

        ModelIndexInfo& top2_info = data.get(top2->index());
        top2_info.expanded = true;
    }

    PositionsCalculator calculator(&data, canvas_w);
    calculator.updateItems();

    //// test
    ModelIndexInfo& top_info = data.get(top->index());
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


TEST_F(PositionsCalculatorShould, FollowDatasThumbnailHeightHint)
{
    //preparations
    const QSize img1(200, 100);
    const QSize img2(100, 500);
    const int canvas_w = 500;

    MockConfiguration config;

    Data data;
    data.set(&model);
    data.setThumbnailDesiredHeight(50);

    const int spacing = data.getSpacing();

    ViewDataModelObserver mo(&data.getModel(), &model);

    const QPixmap pixmap1(img1);
    const QIcon icon1(pixmap1);

    const QPixmap pixmap2(img2);
    const QIcon icon2(pixmap2);

    QStandardItem* child1 = new QStandardItem(icon1, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon2, "Empty2");

    submodel.appendRow(child1);
    submodel.appendRow(child2);

    // setup expectations
    Photo::Data photoDetails1;
    photoDetails1.geometry = img1;
    photoDetails1.id = 0;
    photoDetails1.path = "";

    Photo::Data photoDetails2;
    photoDetails2.geometry = img2;
    photoDetails2.id = 0;
    photoDetails2.path = "";

    const QModelIndex idx1 = child1->index();
    const QModelIndex idx2 = child2->index();

    EXPECT_CALL(model, getPhotoDetails(idx1)).WillRepeatedly(ReturnRef(photoDetails1));
    EXPECT_CALL(model, getPhotoDetails(idx2)).WillRepeatedly(ReturnRef(photoDetails2));
    //

    PositionsCalculator calculator(&data, canvas_w);
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


TEST_F(PositionsCalculatorShould, HandleWideImages)
{
    //preparations
    const QSize img1(1000, 50);
    const QSize img2(100, 50);
    const int canvas_w = 500;

    Data data;
    data.set(&model);
    data.setThumbnailDesiredHeight(50);

    const int spacing = data.getSpacing();
    const int margin  = data.getImageMargin();

    ViewDataModelObserver mo(&data.getModel(), &model);

    const QPixmap pixmap1(img1);
    const QIcon icon1(pixmap1);

    const QPixmap pixmap2(img2);
    const QIcon icon2(pixmap2);

    QStandardItem* child1 = new QStandardItem(icon1, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon2, "Empty2");

    submodel.appendRow(child1);
    submodel.appendRow(child2);

    // setup expectations
    Photo::Data photoDetails1;
    photoDetails1.geometry = img1;
    photoDetails1.id = 0;
    photoDetails1.path = "";

    Photo::Data photoDetails2;
    photoDetails2.geometry = img2;
    photoDetails2.id = 0;
    photoDetails2.path = "";

    const QModelIndex idx1 = child1->index();
    const QModelIndex idx2 = child2->index();

    using ::testing::Return;
    EXPECT_CALL(model, getPhotoDetails(idx1)).WillRepeatedly(ReturnRef(photoDetails1));
    EXPECT_CALL(model, getPhotoDetails(idx2)).WillRepeatedly(ReturnRef(photoDetails2));
    //

    PositionsCalculator calculator(&data, canvas_w);
    calculator.updateItems();

    // Expectations:
    // We expect first image to take whole row.
    // Second image should be moved to next one
    {
        const ModelIndexInfo& info1 = *data.cfind(child1->index());
        EXPECT_EQ(QSize(img1.width() + 2 * spacing, img1.height() + 2 * spacing), info1.getSize());
        EXPECT_EQ(QPoint(margin, 0), info1.getPosition());

        const ModelIndexInfo& info2 = *data.cfind(child2->index());
        EXPECT_EQ(QSize(img2.width() + 2 * spacing, img2.height() + 2 * spacing), info2.getSize());
        EXPECT_EQ(QPoint(margin, img1.height() + 2 * spacing), info2.getPosition());
    }
}



TEST_F(PositionsCalculatorShould, SetChildrenPositionRelativeToParents)
{
    // Situation:
    // two nodes with two children. Children should have
    // positions relative to parents' positions

    const QSize img(100, 50);

    QStandardItem* top_idx1 = new QStandardItem( "Empty" );
    QStandardItem* top1_child1_idx = new QStandardItem( QIcon(QPixmap(img)), "Empty" );

    top_idx1->appendRow(top1_child1_idx);

    QStandardItem* top_idx2 = new QStandardItem( "Empty2" );
    QStandardItem* top2_child1_idx = new QStandardItem( QIcon(QPixmap(img)), "Empty" );

    top_idx2->appendRow(top2_child1_idx);

    submodel.appendRow(top_idx1);
    submodel.appendRow(top_idx2);

    Data view_data;
    view_data.set(&model);

    const int spacing = view_data.getSpacing();
    const int margin  = view_data.getImageMargin();
    const int canvas_w = 500;
    const int thumb_h = view_data.getThumbnailDesiredHeight();
    const int thumb_w = img.width() * thumb_h/img.height();

    // setup expectations
    Photo::Data photoDetails;
    photoDetails.geometry = img;
    photoDetails.id = 0;
    photoDetails.path = "";

    EXPECT_CALL(model, getPhotoDetails(_)).WillRepeatedly(ReturnRef(photoDetails));
    //

    ViewDataModelObserver mo(&view_data.getModel(), &model);

    //expand nodes to show children
    ModelIndexInfo& top_info1 = view_data.get(top_idx1->index());
    top_info1.expanded = true;

    ModelIndexInfo& top_info2 = view_data.get(top_idx2->index());
    top_info2.expanded = true;

    PositionsCalculator calculator(&view_data, canvas_w);
    calculator.updateItems();

    {
        const ModelIndexInfo& info1 = *view_data.cfind(top1_child1_idx->index());
        EXPECT_EQ(QRect(margin, 0, thumb_w + 2 * spacing, thumb_h + 2 * spacing), info1.getRect());

        const ModelIndexInfo& info2 = *view_data.cfind(top2_child1_idx->index());
        EXPECT_EQ(QRect(margin, 0, thumb_w + 2 * spacing, thumb_h + 2 * spacing), info2.getRect());
    }
}
