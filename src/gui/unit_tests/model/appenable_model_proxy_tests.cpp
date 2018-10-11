
#include <gtest/gtest.h>

#include "widgets/tag_editor/helpers/appendable_model_proxy.hpp"


using ::testing::_;
using ::testing::ElementsAre;
using ::testing::InvokeArgument;
using ::testing::Return;


TEST(AppendableModelProxyTest, IsConstructable)
{
    EXPECT_NO_THROW({
        AppendableModelProxy proxy;
    });
}

