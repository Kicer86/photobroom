
#include <gmock/gmock.h>

#include "thumbnail_manager.hpp"


TEST(ThumbnailManagerTest, constructs)
{
    EXPECT_NO_THROW(
    {
        ThumbnailManager {};
    });
}
