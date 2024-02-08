
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


TEST(StoppableTask, notStopped)
{
    int result = 0;
    ConditionalWait waiter;

    std::stop_source stop_source;
    stoppableTask<int>(
        stop_source,
        [](const std::stop_token &, auto c)
        {
            std::jthread([c](const std::stop_token &)
            {
                c(5);
            });
        },
        [&](int v)
        {
            result = v;

            waiter.ready();
        },
        false
    );

    // wait for task to finish
    waiter.wait();

    EXPECT_EQ(result, 5);
}


TEST(StoppableTask, stopped)
{
    bool stopped = false;
    bool called = false;
    ConditionalWait waiter1, waiter2;

    std::stop_source stop_source;
    stoppableTask<int>(
        stop_source,
        [&](const std::stop_token& s, auto c)
        {
            std::thread([&, c, s]
            {
                waiter1.wait();                        // wait for mutex (step #1)

                stopped = s.stop_requested();
                c(5);
                waiter2.ready();                       // release mutex (step #2)
            }).detach();
        },
        [&called](int)
        {
            called = true;
        },
        false
    );

    // step #1: stop task and let it finish
    stop_source.request_stop();
    waiter1.ready();

    // step #2: wait for task to finish
    waiter2.wait();

    // verify expectations
    EXPECT_TRUE(stopped);
    EXPECT_FALSE(called);
}


TEST(StoppableTask, sameThread)
{
    std::string arg1 = "test\0";
    char* argv[] = {arg1.data()};
    int argc = 1;
    QCoreApplication app(argc, argv);

    std::thread::id callbackThreadId;
    const std::thread::id mainThreadId = std::this_thread::get_id();
    ConditionalWait waiter1, waiter2;

    std::stop_source stop_source;
    stoppableTask<int>(
        stop_source,
        [&](const std::stop_token &, auto c)
        {
            std::thread([&, c]
            {
                c(5);
                waiter1.ready();
            }).detach();
        },
        [&](int)
        {
            waiter2.ready([&]{
                callbackThreadId = std::this_thread::get_id();
            });
        }
    );

    // wait for callback to be called
    waiter1.wait();

    // process qt messages
    app.processEvents();

    // wait for callback
    waiter2.wait();

    // verify expectations
    EXPECT_EQ(mainThreadId, callbackThreadId);
}


TEST(StoppableTask, notSameThread)
{
    const std::thread::id mainThreadId = std::this_thread::get_id();
    std::thread::id callbackThreadId = mainThreadId;
    ConditionalWait waiter;

    std::stop_source stop_source;
    stoppableTask<int>(
        stop_source,
        [](const std::stop_token &, auto c)
        {
            std::thread([c]
            {
                c(5);
            }).detach();
        },
        [&](int)
        {
            waiter.ready([&]{
                callbackThreadId = std::this_thread::get_id();
            });
        },
        false
    );

    // wait for callback
    waiter.wait();

    // verify expectations
    EXPECT_NE(mainThreadId, callbackThreadId);
}
