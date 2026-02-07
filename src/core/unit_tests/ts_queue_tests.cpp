
#include <chrono>
#include <future>
#include <memory>
#include <thread>

#include <gtest/gtest.h>

#include "ts_queue.hpp"


TEST(TSQueueTest, supportsMoveOnlyTypes)
{
    ol::TS_Queue<std::unique_ptr<int>> queue(0);

    queue.push(std::make_unique<int>(42));

    auto value = queue.pop();
    ASSERT_TRUE(value.has_value());
    ASSERT_NE(value->get(), nullptr);
    EXPECT_EQ(**value, 42);
}


TEST(TSQueueTest, popForReturnsEmptyAfterTimeout)
{
    ol::TS_Queue<int> queue(0);

    const auto value = queue.pop_for(std::chrono::milliseconds(30));
    EXPECT_FALSE(value.has_value());
}


TEST(TSQueueTest, stopUnblocksPop)
{
    ol::TS_Queue<int> queue(0);

    std::promise<std::optional<int>> resultPromise;
    auto resultFuture = resultPromise.get_future();

    std::thread t([&]
    {
        resultPromise.set_value(queue.pop());
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    queue.stop();

    t.join();

    const auto result = resultFuture.get();
    EXPECT_FALSE(result.has_value());
}


TEST(TSQueueTest, pushBlocksWhenQueueIsFullUntilPopHappens)
{
    ol::TS_Queue<int> queue(1);

    queue.push(1);

    std::promise<void> pushedSecondPromise;
    auto pushedSecondFuture = pushedSecondPromise.get_future();

    std::thread t([&]
    {
        queue.push(2);
        pushedSecondPromise.set_value();
    });

    EXPECT_EQ(pushedSecondFuture.wait_for(std::chrono::milliseconds(30)), std::future_status::timeout);

    const auto first = queue.pop();
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ(*first, 1);

    ASSERT_EQ(pushedSecondFuture.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    t.join();

    const auto second = queue.pop();
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(*second, 2);
}


TEST(TSQueueTest, waitForDataReturnsWhenDataIsPushed)
{
    ol::TS_Queue<int> queue(0);

    std::promise<void> waitedPromise;
    auto waitedFuture = waitedPromise.get_future();

    std::thread t([&]
    {
        queue.wait_for_data();
        waitedPromise.set_value();
    });

    EXPECT_EQ(waitedFuture.wait_for(std::chrono::milliseconds(30)), std::future_status::timeout);

    queue.push(123);

    ASSERT_EQ(waitedFuture.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    t.join();

    const auto value = queue.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, 123);
}


TEST(TSQueueTest, clearMakesQueueEmpty)
{
    ol::TS_Queue<int> queue(0);

    queue.push(1);
    queue.push(2);

    ASSERT_FALSE(queue.empty());
    ASSERT_EQ(queue.size(), 2U);

    queue.clear();

    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0U);
}

