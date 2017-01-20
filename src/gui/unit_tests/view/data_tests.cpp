
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <desktop/views/view_impl/data.hpp>
#include <desktop/views/view_impl/positions_calculator.hpp>
#include <desktop/views/view_impl/positions_translator.hpp>

#include "unit_tests_utils/mock_photo_info.hpp"
#include "test_helpers/mock_configuration.hpp"
#include "test_helpers/mock_qabstractitemmodel.hpp"
#include "test_helpers/photo_info_model.hpp"


class DataShould: public ::testing::Test
{
    public:
        DataShould():
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

TEST_F(DataShould, BeConstructable)
{
    EXPECT_NO_THROW({
        Data data;
    });
}


TEST_F(DataShould, ContainOnlyRootNodeAfterConstruction)
{
    Data data;

    const auto& items = data.getModel();
    EXPECT_EQ(1, items.size());
}


TEST_F(DataShould, ContainOnlyRootNodeAfterClear)
{
    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    const auto& items = data.getModel();
    EXPECT_EQ(1, items.size());
}


TEST_F(DataShould, ReturnEmptyInfoStructWhenAskedAboutNotExistingItem)
{
    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    Data::ModelIndexInfoSet::Model::iterator infoIt = data.get(QModelIndex());
    QModelIndex idx = data.get(infoIt);

    EXPECT_EQ(QModelIndex(), idx);

    const auto& items = data.getModel();
    EXPECT_EQ(false, items.empty());
}


TEST_F(DataShould, SetInitialDataForRootItem)
{
    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    const ModelIndexInfo& info = *data.get(QModelIndex());
    EXPECT_EQ(true, info.expanded);
    EXPECT_EQ(false, info.isPositionValid());
    EXPECT_EQ(false, info.isSizeValid());
    EXPECT_EQ(false, info.isOverallSizeValid());
}


TEST_F(DataShould, StoreInfoAboutItem)
{
    Data data;
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


TEST_F(DataShould, MarkTopItemsAsVisible)
{
    Data data;
    data.set(&model);

    ViewDataModelObserver mo(&data.getModel(), &model);

    QStandardItem* top = new QStandardItem("Empty");
    submodel.appendRow(top);

    QModelIndex top_idx = top->index();

    auto top_it = data.get(top_idx);

    EXPECT_EQ(true, top_it.valid());
    EXPECT_EQ(true, data.isVisible(top_it));
}


TEST_F(DataShould, NotReturnInvisibleItems)
{
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

    submodel.appendRow(top);

    //expand top and update items positions
    ModelIndexInfo& info = *data.get(top->index());
    info.expanded = true;

    // setup expectations
    APhotoInfoModel::PhotoDetails photoDetails;
    photoDetails.size = QSize(10, 10);
    photoDetails.id = 0;
    photoDetails.path = "";

    using ::testing::Return;
    using ::testing::_;
    EXPECT_CALL(model, getPhotoDetails(_)).WillRepeatedly(Return(photoDetails));

    //
    PositionsTranslator translator(&data);

    PositionsCalculator positions_calculator(&data, 100);
    positions_calculator.updateItems();

    const QRect rect1 = translator.getAbsoluteRect(data.get(child1->index()));
    const QRect rect2 = translator.getAbsoluteRect(data.get(child2->index()));

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


TEST_F(DataShould, NotForgetItemSizeWhenParentCollapsedAndExpanded)
{
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

    submodel.appendRow(top);

    // setup expectations
    APhotoInfoModel::PhotoDetails photoDetails;
    photoDetails.size = QSize(10, 10);
    photoDetails.id = 0;
    photoDetails.path = "";

    using ::testing::Return;
    using ::testing::_;
    EXPECT_CALL(model, getPhotoDetails(_)).WillRepeatedly(Return(photoDetails));
    //

    //expand top and update items positions
    ModelIndexInfo& info = *data.get(top->index());
    info.expanded = true;

    PositionsCalculator positions_calculator(&data, 100);
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


TEST_F(DataShould, HideChildrenOfCollapsedNode)
{
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

    submodel.appendRow(top);

    // setup expectations
    APhotoInfoModel::PhotoDetails photoDetails;
    photoDetails.size = QSize(10, 10);
    photoDetails.id = 0;
    photoDetails.path = "";

    using ::testing::Return;
    using ::testing::_;
    EXPECT_CALL(model, getPhotoDetails(_)).WillRepeatedly(Return(photoDetails));
    //

    //expand top and update items positions
    ModelIndexInfo& info = *data.get(top->index());
    info.expanded = true;

    PositionsCalculator positions_calculator(&data, 100);
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


TEST_F(DataShould, ReturnProperIndicesOfItems)
{
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

    submodel.appendRow(top);

    // setup expectations
    APhotoInfoModel::PhotoDetails photoDetails;
    photoDetails.size = QSize(10, 10);
    photoDetails.id = 0;
    photoDetails.path = "";

    using ::testing::Return;
    using ::testing::_;
    EXPECT_CALL(model, getPhotoDetails(_)).WillRepeatedly(Return(photoDetails));
    //

    //expand top so children will be stored in 'data' when calculating positions
    auto it = data.get(top->index());
    it->expanded = true;

    PositionsCalculator positions_calculator(&data, 100);
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


TEST_F(DataShould, ResizeImageAccordinglyToThumbnailHeightHint)
{
    //preparations
    const QSize img1(200, 100);
    const QSize img2(100, 500);
    const int canvas_w = 500;

    Data data;
    data.set(&model);
    data.setThumbnailDesiredHeight(50);

    ViewDataModelObserver mo(&data.getModel(), &model);

    const QPixmap pixmap1(img1);
    const QIcon icon1(pixmap1);

    const QPixmap pixmap2(img2);
    const QIcon icon2(pixmap2);

    QStandardItem* child1 = new QStandardItem(icon1, "Empty1");
    QStandardItem* child2 = new QStandardItem(icon2, "Empty2");

    submodel.appendRow(child1);
    submodel.appendRow(child2);

    const QModelIndex idx1 = child1->index();
    const QModelIndex idx2 = child2->index();

    // setup expectations
    APhotoInfoModel::PhotoDetails photoDetails1;
    photoDetails1.size = img1;
    photoDetails1.id = 1;
    photoDetails1.path = "";

    APhotoInfoModel::PhotoDetails photoDetails2;
    photoDetails2.size = img2;
    photoDetails2.id = 2;
    photoDetails2.path = "";

    using ::testing::Return;
    using ::testing::_;
    EXPECT_CALL(model, getPhotoDetails(idx1)).WillRepeatedly(Return(photoDetails1));
    EXPECT_CALL(model, getPhotoDetails(idx2)).WillRepeatedly(Return(photoDetails2));
    //

    PositionsCalculator calculator(&data, canvas_w);
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
        EXPECT_EQ(img1, pix1Size);                      // original

        const auto child2It = data.get(child2->index());
        const QSize thumb2 = data.getThumbnailSize(child2It);
        const QPixmap pix2 = data.getImage(child2It);
        const QSize pix2Size = pix2.size();

        EXPECT_EQ(QSize(10, 50), thumb2);               // scaled
        EXPECT_EQ(img2, pix2Size);                      // original
    }
}
