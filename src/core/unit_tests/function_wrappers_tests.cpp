
#include <functional>
#include <gmock/gmock.h>

#include "function_wrappers.hpp"


using namespace std::placeholders;

struct DummyCallback
{
    MOCK_METHOD(void, method1, ());
    MOCK_METHOD(void, method2, (int, int));
};

TEST(SafeCallbackTest, simpleCall)
{
    DummyCallback callback;
    EXPECT_CALL(callback, method1).Times(1);

    safe_callback_ctrl controller;

    auto safe_callback = controller.make_safe_callback<>(std::bind(&DummyCallback::method1, &callback));

    EXPECT_TRUE(safe_callback.is_valid());
    safe_callback();
}


TEST(SafeCallbackTest, disconnectedCall)
{
    DummyCallback callback;
    EXPECT_CALL(callback, method1).Times(0);

    safe_callback_ctrl controller;

    auto safe_callback = controller.make_safe_callback<>(std::bind(&DummyCallback::method1, &callback));
    controller.invalidate();

    EXPECT_FALSE(safe_callback.is_valid());
    safe_callback();
}


TEST(SafeCallbackTest, callWithArguments)
{
    DummyCallback callback;
    EXPECT_CALL(callback, method2(123, 456)).Times(1);

    safe_callback_ctrl controller;

    auto safe_callback = controller.make_safe_callback<int, int>(std::bind(&DummyCallback::method2, &callback, _1, _2));

    EXPECT_TRUE(safe_callback.is_valid());
    safe_callback(123, 456);
}


TEST(SafeCallbackTest, callToBadFunction)
{
    safe_callback_ctrl controller;

    auto safe_callback = controller.make_safe_callback<>(std::function<void()>());

    EXPECT_FALSE(safe_callback.is_valid());
    safe_callback();
}


TEST(SafeCallbackTest, sequentialCall)
{
    DummyCallback callback;
    EXPECT_CALL(callback, method1).Times(0);
    EXPECT_CALL(callback, method2(123, 456)).Times(1);

    safe_callback_ctrl controller;
    auto safe_callback1 = controller.make_safe_callback<>(std::bind(&DummyCallback::method1, &callback));
    controller.invalidate();
    auto safe_callback2 = controller.make_safe_callback<int, int>(std::bind(&DummyCallback::method2, &callback, _1, _2));

    EXPECT_FALSE(safe_callback1.is_valid());
    EXPECT_TRUE(safe_callback2.is_valid());
    safe_callback1();
    safe_callback2(123, 456);
}
