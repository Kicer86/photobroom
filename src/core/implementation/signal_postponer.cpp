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

#include "signal_postponer.hpp"


void SignalPostponer::setDelay(const std::chrono::milliseconds& d)
{
    m_delay = d;
}


void SignalPostponer::setPatiece(const std::chrono::milliseconds& p)
{
    m_patience = p;
}


void SignalPostponer::notify()
{
    if (m_lazinessTimer.isActive() == false)        // lazy timer will be launched for the first time?
        m_patienceTimer.start(m_patience.count());  // start patience timer also

    m_lazinessTimer.start(m_delay.count());
}


void SignalPostponer::stop()
{
    m_lazinessTimer.stop();
    m_patienceTimer.stop();
}


SignalBlocker::SignalBlocker(std::chrono::milliseconds blockTime, QObject* p)
    : QObject(p)
    , m_dirty(false)
    , m_locked(false)
{
    m_timer.setSingleShot(true);
    m_timer.setInterval(blockTime);

    connect(&m_timer, &QTimer::timeout, this, &SignalBlocker::unlock);
}


void SignalBlocker::notify()
{
    std::lock_guard<std::recursive_mutex> _(m_mutex);
    m_dirty = true;

    tryFire();
}


void SignalBlocker::tryFire()
{
    std::lock_guard<std::recursive_mutex> _(m_mutex);

    if (m_locked == false && m_dirty)
    {
        m_dirty = false;
        auto locker = lock();

        emit fire(locker);
    }
}


SignalBlocker::Locker SignalBlocker::lock()
{
    std::lock_guard<std::recursive_mutex> _(m_mutex);
    assert(m_locked == false);

    QObject* obj = new QObject(this);
    connect(obj, &QObject::destroyed, &m_timer, qOverload<>(&QTimer::start));

    m_locked = true;

    return QSharedPointer<QObject>(obj);
}


void SignalBlocker::unlock()
{
    std::lock_guard<std::recursive_mutex> _(m_mutex);
    assert(m_locked);

    m_locked = false;
    tryFire();
}
