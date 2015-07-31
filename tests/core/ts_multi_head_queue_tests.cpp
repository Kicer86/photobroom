
#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <core/ts_multi_head_queue.hpp>


TEST(TS_MultiHeadQueueTest, isConstructible)
{
    EXPECT_NO_THROW(
        {
            TS_MultiHeadQueue<int> q;
        });
}

