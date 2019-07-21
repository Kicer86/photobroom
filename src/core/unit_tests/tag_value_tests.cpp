
#include <gmock/gmock.h>

#include "tag.hpp"

TEST(TagValueTest, EmptyAfterConstruciton)
{
    const TagValue tv;

    EXPECT_EQ(tv.type(), TagValue::Type::Empty);
}
