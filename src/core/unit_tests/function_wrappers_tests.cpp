
#include <functional>
#include <thread>
#include <gmock/gmock.h>
#include <QCoreApplication>

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


TEST(StoppableTask, notStopped)
{
    int result = 0;
    std::mutex mutex;

    mutex.lock();

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
        [&result, &mutex](int v)
        {
            result = v;
            mutex.unlock();
        },
        false
    );

    // wait for task to finish
    mutex.lock();

    EXPECT_EQ(result, 5);
}


TEST(StoppableTask, stopped)
{
    bool stopped = false;
    bool called = false;
    std::mutex mutex_1, mutex_2;

    mutex_1.lock();
    mutex_2.lock();

    std::stop_source stop_source;
    stoppableTask<int>(
        stop_source,
        [&stopped, &mutex_1, &mutex_2](const std::stop_token& s, auto c)
        {
            std::thread([c, s, &stopped, &mutex_1, &mutex_2]
            {
                mutex_1.lock();                     // wait for mutex (step #1)
                stopped = s.stop_requested();
                c(5);
                mutex_2.unlock();                   // release mutex (step #2)
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
    mutex_1.unlock();

    // step #2: wait for task to finish
    mutex_2.lock();

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
    std::mutex mutex_1, mutex_2;

    mutex_1.lock();
    mutex_2.lock();

    std::stop_source stop_source;
    stoppableTask<int>(
        stop_source,
        [&mutex_1](const std::stop_token &, auto c)
        {
            std::thread([c, &mutex_1]
            {
                c(5);
                mutex_1.unlock();
            }).detach();
        },
        [&mutex_2, &callbackThreadId](int)
        {
            callbackThreadId = std::this_thread::get_id();
            mutex_2.unlock();
        }
    );

    // wait for callback to be called
    mutex_1.lock();

    // process qt messages
    app.processEvents();

    // wait for callback
    mutex_2.lock();

    // verify expectations
    EXPECT_EQ(mainThreadId, callbackThreadId);
}


TEST(StoppableTask, notSameThread)
{
    std::thread::id callbackThreadId;
    const std::thread::id mainThreadId = std::this_thread::get_id();
    std::mutex mutex;

    mutex.lock();;

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
        [&mutex, &callbackThreadId](int)
        {
            callbackThreadId = std::this_thread::get_id();
            mutex.unlock();
        },
        false
    );

    // wait for callback
    mutex.lock();

    // verify expectations
    EXPECT_NE(mainThreadId, callbackThreadId);
}
