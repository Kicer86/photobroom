/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SIGNALPOSTPONER_HPP
#define SIGNALPOSTPONER_HPP

#include <chrono>
#include <mutex>
#include <optional>

#include <QObject>
#include <QTimer>
#include <QSharedPointer>

#include "core_export.h"

/** @brief Helper for @ref lazy_connect. Not meant to be used directly. */
class CORE_EXPORT SignalPostponer: public QObject
{
     Q_OBJECT

    public:
        template<typename Fire>
        explicit SignalPostponer(Fire fire, QObject* p):
            QObject(p),
            m_delay(std::chrono::milliseconds(250)),
            m_patience(std::chrono::milliseconds(1000))
        {
            connect(&m_lazinessTimer, &QTimer::timeout, fire);
            connect(&m_patienceTimer, &QTimer::timeout, fire);

            // when any of timers timeouts, stop both of them
            connect(&m_lazinessTimer, &QTimer::timeout, this, &SignalPostponer::stop);
            connect(&m_patienceTimer, &QTimer::timeout, this, &SignalPostponer::stop);

            m_lazinessTimer.setSingleShot(true);
            m_patienceTimer.setSingleShot(true);
        }

        ~SignalPostponer() = default;

        void setDelay(const std::chrono::milliseconds &);
        void setPatiece(const std::chrono::milliseconds &);
        void notify();

    private:
        QTimer m_lazinessTimer;
        QTimer m_patienceTimer;
        std::chrono::milliseconds m_delay;
        std::chrono::milliseconds m_patience;

        void stop();
};


/** @brief Create lazy Qt connection between signal and slot.
 *
 * Function creates 'lazy' connection (similar to Qt's QObject::connect).
 * Main goal is to group frequently emitted signals and call slot once.
 * When signal is emitted, lazy connection will wait for @p delay milliseconds.
 * If no signal is emmited in that time, @p slot will be invoked.
 * Otherwise lazy connection will wait again @p delay milliseconds,
 * for another signal emission. If signal is being emitted
 * over and over @p patience is the final, total awaiting time. When
 * excited, slot will be invoked.
 *
 * Please mind that lazy_connect will create an instance of a SignalPostponer
 * object attached as a child to @p src object. If @p src object is
 * destroyed no slots will be invoked even when signal was emitted before
 * object deletion.
 *
 * @param src source object.
 * @param sig signal.
 * @param dst destination object.
 * @param slot slot.
 * @param delay minimal delay between signal emission and slot invocation.
 * @param patience maximal delay between signal emission and slot invocation.
 */
template<typename SrcObj, typename Signal, typename Dst, typename Slot>
void lazy_connect(SrcObj* src, Signal sig,
                  Dst* dst, Slot slot,
                  const std::chrono::milliseconds& delay = std::chrono::milliseconds(250),
                  const std::chrono::milliseconds& patience = std::chrono::milliseconds(1000),
                  Qt::ConnectionType type = Qt::AutoConnection)
{
    SignalPostponer* postponer = new SignalPostponer([dst, slot]()
    {
        // launch destination slot
        (dst->*slot)();
    },
    src);

    postponer->setDelay(delay);
    postponer->setPatiece(patience);

    QObject::connect(src, sig, postponer, &SignalPostponer::notify, type);
}


/** @brief Helper class for @ref blocked_connect. Not meant to be use directly. */
class CORE_EXPORT SignalBlocker: public QObject
{
    Q_OBJECT

public:
    typedef QSharedPointer<QObject> Locker;

    explicit SignalBlocker(std::chrono::milliseconds blockTime, QObject* parent = nullptr);
    void notify();

signals:
    void fire(Locker) const;

private:
    std::recursive_mutex m_mutex;
    std::chrono::milliseconds m_blockTime;
    bool m_dirty;
    bool m_locked;

    void tryFire();
    Locker lock();
    void unlock();
};


/**
 * @brief blocks slot invocation until previous invocation is complete.
 *
 * Function creates @p blocked connection (similar to Qt's QObject::connect).
 * Goal of blocked connection is to prevent calling slot faster than it executes.
 * When signal is emitted slot will be invoked, but connection becomes
 * suspended until slot finishes execution. If signal was emitted many
 * times during that period, slot will be called only once.
 * Blocked connection passes extra object to invoked slot. As long as
 * this object (or a copy of it) exists, slot invocation is suspended.
 * Suspension time can be additionaly extendend with @p block parameter:
 * when last copy of object passed to slot is destroyed,
 * connection will be suspended for addiotional @p block milliseconds.
 *
 * @param src source object.
 * @param sig signal.
 * @param dst destination object.
 * @param slot slot.
 * @param block addiotional suspension time in milliseconds.
 */
template<typename SrcObj, typename Signal, typename Dst, typename Slot>
SignalBlocker* blocked_connect(SrcObj* src, Signal sig,
                               Dst* dst, Slot slot,
                               const std::chrono::milliseconds& block = std::chrono::milliseconds(0),
                               Qt::ConnectionType type = Qt::AutoConnection)
{
    SignalBlocker* blocker = new SignalBlocker(block, src);
    QObject::connect(src, sig, blocker, &SignalBlocker::notify);
    QObject::connect(blocker, &SignalBlocker::fire, dst, slot, type);

    return blocker;
}

#endif // SIGNALPOSTPONER_HPP
