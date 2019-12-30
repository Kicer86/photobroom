/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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
 *
 */

#include "signal_filter.hpp"

#include <cassert>

#include <QTimer>


Receiver::Receiver(QObject* parent_object, const std::function<void()>& target, const std::chrono::milliseconds& ms):
    QObject(parent_object),
    m_block_time(ms),
    m_target(target),
    m_blocked(false),
    m_dirty(false)
{

}


void Receiver::notification()
{
    if (m_blocked)
        m_dirty = true;
    else
    {
        m_blocked = true;
        const int64_t ms = m_block_time.count();
        QTimer::singleShot(static_cast<int>(ms), this, &Receiver::clear);

        m_target();
    }
}


void Receiver::clear()
{
    assert(m_blocked);

    m_blocked = false;

    if (m_dirty)
    {
        m_dirty = false;
        notification();
    }
}


SignalFilter::SignalFilter(QObject* parent_object): QObject(parent_object)
{

}


SignalFilter::~SignalFilter()
{

}


void SignalFilter::connect(QObject* sender_obj, const char* signal, const std::function<void()>& target, const std::chrono::milliseconds& ms, Qt::ConnectionType type)
{
    Receiver* rec = new Receiver(this, target, ms);
    QObject::connect(sender_obj, signal, rec, SLOT(notification()), type);
}
