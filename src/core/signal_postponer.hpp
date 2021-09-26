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

/** @brief Helper for @ref lazy_connect. Not meant to be used directly */
class CORE_EXPORT SignalPostponer: public QObject
{
     Q_OBJECT

    public:
        template<typename Fire>
        SignalPostponer(Fire fire, QObject* p):
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


template<typename SrcObj, typename Signal1, typename Dst, typename Signal2>
void lazy_connect(SrcObj* src, Signal1 sig1,
                  Dst* dst, Signal2 slot1,
                  const std::chrono::milliseconds& delay = std::chrono::milliseconds(250),
                  const std::chrono::milliseconds& patience = std::chrono::milliseconds(1000),
                  Qt::ConnectionType type = Qt::AutoConnection)
{
    SignalPostponer* postponer = new SignalPostponer([dst, slot1]()
        {
            // launch destination slot
            (dst->*slot1)();
        },
        src);

    postponer->setDelay(delay);
    postponer->setPatiece(patience);

    QObject::connect(src, sig1, postponer, &SignalPostponer::notify, type);
}


class CORE_EXPORT SignalBlocker: public QObject
{
    Q_OBJECT

public:
    typedef QSharedPointer<QObject> Locker;

    SignalBlocker(std::chrono::milliseconds blockTime, QObject* parent = nullptr);
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


template<typename SrcObj, typename Signal1, typename Dst, typename Signal2>
SignalBlocker* blocked_connect(SrcObj* src, Signal1 sig1,
                               Dst* dst, Signal2 slot1,
                               const std::chrono::milliseconds& block = std::chrono::milliseconds(0),
                               Qt::ConnectionType type = Qt::AutoConnection)
{
    SignalBlocker* blocker = new SignalBlocker(block, src);
    QObject::connect(src, sig1, blocker, &SignalBlocker::notify);
    QObject::connect(blocker, &SignalBlocker::fire, dst, slot1, type);

    return blocker;
}

#endif // SIGNALPOSTPONER_HPP
