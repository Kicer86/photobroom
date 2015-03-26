
#include <gtest/gtest.h>

#include <desktop/model_view/model_helpers/idx_data.hpp>
#include <desktop/model_view/model_helpers/idx_data_manager.hpp>
#include <desktop/components/photos_data_model.hpp>


class IdxDataTest : public ::testing::Test
{
    public:
        IdxDataTest(): model(), manager(&model) {}

    protected:
        PhotosDataModel model;
        IdxDataManager manager;
};



TEST_F(IdxDataTest, IsConstructible)
{
    EXPECT_NO_THROW({
        IdxData item(&manager, "");
    });
}


TEST_F(IdxDataTest, addsChildrenAtRightPositions)
{
    IdxData root_item(&manager, "root");

    IdxData* child1 = new IdxData(&manager, "child1");
    IdxData* child2 = new IdxData(&manager, "child2");
    IdxData* child3 = new IdxData(&manager, "child3");
    IdxData* child4 = new IdxData(&manager, "child4");

    //place in a "random" order
    root_item.addChild(child4);
    root_item.addChild(child2);
    root_item.addChild(child1);
    root_item.addChild(child3);

    EXPECT_EQ( 0, root_item.getPositionOf(child1) );
    EXPECT_EQ( 1, root_item.getPositionOf(child2) );
    EXPECT_EQ( 2, root_item.getPositionOf(child3) );
    EXPECT_EQ( 3, root_item.getPositionOf(child4) );

    EXPECT_EQ( nullptr, root_item.findChildWithBadPosition() );
}


TEST_F(IdxDataTest, equalChilrenShouldBeAppended)
{
    IdxData root_item(&manager, "root");

    IdxData* child1 = new IdxData(&manager, "child");
    IdxData* child2 = new IdxData(&manager, "child");
    IdxData* child3 = new IdxData(&manager, "child");
    IdxData* child4 = new IdxData(&manager, "child");

    root_item.addChild(child1);
    root_item.addChild(child2);
    root_item.addChild(child3);
    root_item.addChild(child4);

    // Placing order should be the same as read order
    // This will guarantee minimal reorganizations in model
    EXPECT_EQ( 0, root_item.getPositionOf(child1) );
    EXPECT_EQ( 1, root_item.getPositionOf(child2) );
    EXPECT_EQ( 2, root_item.getPositionOf(child3) );
    EXPECT_EQ( 3, root_item.getPositionOf(child4) );

    EXPECT_EQ( nullptr, root_item.findChildWithBadPosition() );
}
