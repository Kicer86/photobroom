
#include <functional>
#include <thread>
#include <gmock/gmock.h>
#include <QCoreApplication>

#include "function_wrappers.hpp"


namespace
{
    class ConditionalWait
    {
        std::mutex mutex;
        std::condition_variable cv;
        bool done = false;

    public:
        void wait()
        {
            std::unique_lock lk(mutex);
            cv.wait(lk, [this]{ return done; });
        }

        void ready()
        {
            std::unique_lock lk(mutex);
            done = true;
            cv.notify_one();
        }

        template<typename C>
        requires std::is_invocable_v<C>
        void ready(C c)
        {
            std::unique_lock lk(mutex);
            c();
            done = true;
            lk.unlock();
            cv.notify_one();
        }
    };
}


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
