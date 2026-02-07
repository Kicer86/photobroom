
#include <chrono>

#include <gtest/gtest.h>

#include <QElapsedTimer>
#include <QEventLoop>
#include <QObject>
#include <QTimer>

#include "signal_postponer.hpp"


namespace
{
    struct EventLoopGuard final
    {
        explicit EventLoopGuard(QEventLoop& loop, std::chrono::milliseconds timeout)
        {
            m_timeoutTimer.setSingleShot(true);
            m_timeoutTimer.setInterval(static_cast<int>(timeout.count()));
            QObject::connect(&m_timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
            m_timeoutTimer.start();
        }

    private:
        QTimer m_timeoutTimer;
    };

    class LazyConnectReceiver final: public QObject
    {
    public:
        QEventLoop* loop = nullptr;
        int fired = 0;

        void onFired()
        {
            ++fired;
            if (loop)
                loop->quit();
        }
    };

    class BlockedConnectReceiver final: public QObject
    {
    public:
        QEventLoop* loop = nullptr;
        QTimer* src = nullptr;

        int fired = 0;
        SignalBlocker::Locker locker;

        void onFired(SignalBlocker::Locker l)
        {
            ++fired;
            locker = l;

            if (fired == 1 && loop)
                QTimer::singleShot(50, loop, [&] { locker.reset(); });

            if (fired == 2)
            {
                if (src)
                    src->stop();

                if (loop)
                    loop->quit();
            }
        }
    };
}


TEST(SignalPostponerTest, firesOnceAfterDelay)
{
    QEventLoop loop;
    [[maybe_unused]] EventLoopGuard guard(loop, std::chrono::milliseconds(1000));

    int fired = 0;
    QElapsedTimer timer;

    SignalPostponer postponer([&]
    {
        ++fired;
        loop.quit();
    }, nullptr);

    const std::chrono::milliseconds delay(30);
    const std::chrono::milliseconds patience(500);

    postponer.setDelay(delay);
    postponer.setPatience(patience);

    timer.start();
    postponer.notify();
    loop.exec();

    ASSERT_EQ(fired, 1);
    EXPECT_LT(timer.elapsed(), static_cast<qint64>(patience.count() * 2));
}


TEST(SignalPostponerTest, coalescesNotificationsUntilNoMoreCome)
{
    QEventLoop loop;
    [[maybe_unused]] EventLoopGuard guard(loop, std::chrono::milliseconds(1000));

    QElapsedTimer timer;
    timer.start();

    std::chrono::milliseconds delay(60);
    std::chrono::milliseconds patience(800);

    qint64 lastNotifyMs = -1;
    qint64 firedAtMs = -1;
    int fired = 0;

    SignalPostponer postponer([&]
    {
        firedAtMs = timer.elapsed();
        ++fired;
        loop.quit();
    }, nullptr);

    postponer.setDelay(delay);
    postponer.setPatience(patience);

    const auto notify = [&]
    {
        lastNotifyMs = timer.elapsed();
        postponer.notify();
    };

    notify();
    QTimer::singleShot(10, &loop, notify);
    QTimer::singleShot(20, &loop, notify);

    loop.exec();

    ASSERT_EQ(fired, 1);
    ASSERT_GE(lastNotifyMs, 0);
    ASSERT_GE(firedAtMs, 0);
    EXPECT_GE(firedAtMs, lastNotifyMs + static_cast<qint64>(delay.count()) - 20);
}


TEST(SignalPostponerTest, firesDueToPatienceDespiteContinuousNotifications)
{
    QEventLoop loop;
    [[maybe_unused]] EventLoopGuard guard(loop, std::chrono::milliseconds(1000));

    QElapsedTimer timer;
    timer.start();

    const std::chrono::milliseconds delay(1000);
    const std::chrono::milliseconds patience(80);

    qint64 firedAtMs = -1;
    int fired = 0;

    SignalPostponer postponer([&]
    {
        firedAtMs = timer.elapsed();
        ++fired;
        loop.quit();
    }, nullptr);

    postponer.setDelay(delay);
    postponer.setPatience(patience);

    QTimer spammer;
    spammer.setInterval(10);
    QObject::connect(&spammer, &QTimer::timeout, &postponer, &SignalPostponer::notify);

    spammer.start();
    postponer.notify();

    loop.exec();
    spammer.stop();

    ASSERT_EQ(fired, 1);
    ASSERT_GE(firedAtMs, 0);
    EXPECT_LT(firedAtMs, static_cast<qint64>(delay.count()));
}


TEST(SignalPostponerTest, doesNotFireTwiceWhenDelayEqualsPatience)
{
    QEventLoop loop;
    [[maybe_unused]] EventLoopGuard guard(loop, std::chrono::milliseconds(1000));

    int fired = 0;

    SignalPostponer postponer([&]
    {
        ++fired;
    }, nullptr);

    const std::chrono::milliseconds delay(50);
    const std::chrono::milliseconds patience(50);

    postponer.setDelay(delay);
    postponer.setPatience(patience);

    postponer.notify();

    QTimer::singleShot(200, &loop, &QEventLoop::quit);
    loop.exec();

    EXPECT_EQ(fired, 1);
}


TEST(SignalBlockerTest, blocksUntilLockerIsDestroyedAndBlockTimePasses)
{
    QEventLoop loop;
    [[maybe_unused]] EventLoopGuard guard(loop, std::chrono::milliseconds(1000));

    SignalBlocker blocker(std::chrono::milliseconds(30));

    int fired = 0;
    SignalBlocker::Locker locker;

    QObject::connect(&blocker, &SignalBlocker::fire, &loop, [&](SignalBlocker::Locker l)
    {
        ++fired;
        locker = l;

        if (fired == 2)
            loop.quit();
    });

    blocker.notify();
    ASSERT_EQ(fired, 1);

    blocker.notify();
    blocker.notify();
    ASSERT_EQ(fired, 1);

    locker.reset();
    loop.exec();

    EXPECT_EQ(fired, 2);
}


TEST(SignalBlockerTest, doesNotRefireWithoutExtraNotifications)
{
    QEventLoop loop;
    [[maybe_unused]] EventLoopGuard guard(loop, std::chrono::milliseconds(300));

    SignalBlocker blocker(std::chrono::milliseconds(30));

    int fired = 0;
    SignalBlocker::Locker locker;

    QObject::connect(&blocker, &SignalBlocker::fire, &loop, [&](SignalBlocker::Locker l)
    {
        ++fired;
        locker = l;
    });

    blocker.notify();
    ASSERT_EQ(fired, 1);

    locker.reset();

    loop.exec();
    EXPECT_EQ(fired, 1);
}


TEST(LazyConnectTest, firesWithPatienceWhenSignalsKeepComing)
{
    QEventLoop loop;
    [[maybe_unused]] EventLoopGuard guard(loop, std::chrono::milliseconds(1000));

    QElapsedTimer timer;
    timer.start();

    QTimer src;
    src.setInterval(10);
    src.setSingleShot(false);

    QObject dst;

    int fired = 0;
    qint64 firedAtMs = -1;

    const std::chrono::milliseconds delay(1000);
    const std::chrono::milliseconds patience(80);

    lazy_connect(&src, &QTimer::timeout, &dst, [&]
    {
        firedAtMs = timer.elapsed();
        ++fired;
        src.stop();
        loop.quit();
    }, delay, patience);

    src.start();
    loop.exec();

    ASSERT_EQ(fired, 1);
    ASSERT_GE(firedAtMs, 0);
    EXPECT_LT(firedAtMs, static_cast<qint64>(delay.count()));
}


TEST(BlockedConnectTest, firesAgainWhenLockerIsDestroyed)
{
    QEventLoop loop;
    [[maybe_unused]] EventLoopGuard guard(loop, std::chrono::milliseconds(1000));

    QTimer src;
    src.setInterval(10);
    src.setSingleShot(false);

    QObject dst;

    int fired = 0;
    SignalBlocker::Locker locker;

    blocked_connect(&src, &QTimer::timeout, &dst, [&](SignalBlocker::Locker l)
    {
        ++fired;

        if (fired == 1)
        {
            locker = l;
            QTimer::singleShot(50, &loop, [&] { locker.reset(); });
        }
        else if (fired == 2)
        {
            src.stop();
            loop.quit();
        }
    }, std::chrono::milliseconds(30));

    src.start();
    loop.exec();

    EXPECT_EQ(fired, 2);
}


TEST(LazyConnectTest, supportsMemberFunctionSlot)
{
    QEventLoop loop;
    [[maybe_unused]] EventLoopGuard guard(loop, std::chrono::milliseconds(1000));

    QTimer src;
    src.setInterval(1);
    src.setSingleShot(true);

    LazyConnectReceiver receiver;
    receiver.loop = &loop;

    lazy_connect(&src, &QTimer::timeout, &receiver, &LazyConnectReceiver::onFired,
                 std::chrono::milliseconds(20), std::chrono::milliseconds(200));

    src.start();
    loop.exec();

    EXPECT_EQ(receiver.fired, 1);
}


TEST(BlockedConnectTest, supportsMemberFunctionSlot)
{
    QEventLoop loop;
    [[maybe_unused]] EventLoopGuard guard(loop, std::chrono::milliseconds(1000));

    QTimer src;
    src.setInterval(10);
    src.setSingleShot(false);

    BlockedConnectReceiver receiver;
    receiver.loop = &loop;
    receiver.src = &src;

    blocked_connect(&src, &QTimer::timeout, &receiver, &BlockedConnectReceiver::onFired,
                    std::chrono::milliseconds(30));

    src.start();
    loop.exec();

    EXPECT_EQ(receiver.fired, 2);
}

