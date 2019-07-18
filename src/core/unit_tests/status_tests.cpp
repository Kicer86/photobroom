
#include <gmock/gmock.h>

#include "status.hpp"


TEST(StatusTest, okByDefault)
{
    Status<int, 1> s;

    EXPECT_TRUE(s);
}

TEST(StatusTest, returnStatus)
{
    Status<int, 1> s;
    EXPECT_EQ(s.get(), 1);    // return ok

    s = 123;
    EXPECT_EQ(s.get(), 123);
}


TEST(StatusTest, returnFalseWhenNotOk)
{
    Status<int, 1> s(5);
    EXPECT_FALSE(s);

    s = 0;
    EXPECT_FALSE(s);
}
