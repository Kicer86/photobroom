
#include <gmock/gmock.h>

#include "model_compositor.hpp"

TEST(ModelCompositorTest, emptiness)
{
    ModelCompositor model_compositor;

    EXPECT_EQ(model_compositor.rowCount(), 0);
}
