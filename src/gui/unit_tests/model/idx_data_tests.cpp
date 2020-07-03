
#include <gtest/gtest.h>

#include <desktop/models/model_helpers/idx_data.hpp>
#include <desktop/models/model_helpers/idx_data_manager.hpp>
#include <desktop/models/db_data_model.hpp>

#include "unit_tests_utils/printers.hpp"


class IdxDataTest : public ::testing::Test
{
    public:
        IdxDataTest(): model(), manager(&model) {}

    protected:
        DBDataModel model;
        IdxDataManager manager;
};


TEST_F(IdxDataTest, IsConstructible)
{
    EXPECT_NO_THROW({
        IdxNodeData item(&manager, "");
    });
}


TEST_F(IdxDataTest, addsChildrenAtRightPositions)
{
    IdxNodeData root_item(&manager, "root");

    auto child1_ptr = std::make_unique<IdxNodeData>(&manager, "child1");
    auto child2_ptr = std::make_unique<IdxNodeData>(&manager, "child2");
    auto child3_ptr = std::make_unique<IdxNodeData>(&manager, "child3");
    auto child4_ptr = std::make_unique<IdxNodeData>(&manager, "child4");

    //place in a "random" order
    IIdxData* child4 = root_item.addChild(std::move(child4_ptr));
    IIdxData* child2 = root_item.addChild(std::move(child2_ptr));
    IIdxData* child1 = root_item.addChild(std::move(child1_ptr));
    IIdxData* child3 = root_item.addChild(std::move(child3_ptr));

    EXPECT_EQ( 0, root_item.getPositionOf(child1) );
    EXPECT_EQ( 1, root_item.getPositionOf(child2) );
    EXPECT_EQ( 2, root_item.getPositionOf(child3) );
    EXPECT_EQ( 3, root_item.getPositionOf(child4) );

    EXPECT_EQ( nullptr, root_item.findChildWithBadPosition() );
}


TEST_F(IdxDataTest, equalChilrenShouldBeAppended)
{
    IdxNodeData root_item(&manager, "root");

    auto child1_ptr = std::make_unique<IdxNodeData>(&manager, "child");
    auto child2_ptr = std::make_unique<IdxNodeData>(&manager, "child");
    auto child3_ptr = std::make_unique<IdxNodeData>(&manager, "child");
    auto child4_ptr = std::make_unique<IdxNodeData>(&manager, "child");

    IIdxData* child1 = root_item.addChild(std::move(child1_ptr));
    IIdxData* child2 = root_item.addChild(std::move(child2_ptr));
    IIdxData* child3 = root_item.addChild(std::move(child3_ptr));
    IIdxData* child4 = root_item.addChild(std::move(child4_ptr));

    // Placing order should be the same as read order
    // This will guarantee minimal reorganizations in model
    EXPECT_EQ( 0, root_item.getPositionOf(child1) );
    EXPECT_EQ( 1, root_item.getPositionOf(child2) );
    EXPECT_EQ( 2, root_item.getPositionOf(child3) );
    EXPECT_EQ( 3, root_item.getPositionOf(child4) );

    EXPECT_EQ( nullptr, root_item.findChildWithBadPosition() );
}
