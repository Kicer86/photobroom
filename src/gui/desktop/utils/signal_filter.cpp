/*
 * Tool for limiting frequent signals.
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

#include <QSignalMapper>


SignalFilter::SignalFilter(QObject* parent_object): QObject(parent_object), m_signals(), m_mapper(new QSignalMapper(this))
{

}


SignalFilter::~SignalFilter()
{

}


void SignalFilter::connect(QObject* sender_obj, const char* signal, QObject* receiver, const char* method, Qt::ConnectionType type)
{
    Receiver rec = {receiver, method};
    m_signals[sender_obj] = rec;
    m_mapper->setMapping(sender_obj, sender_obj);
    QObject::connect(sender_obj, signal, m_mapper, SLOT(map()), type);
}


void SignalFilter::notification(QObject* obj)
{
    auto it = m_signals.find(obj);
    assert(it != m_signals.end());

    if (it != m_signals.end())
    {
        const Receiver& r = it->second;

        QMetaObject::invokeMethod(r.receiver, r.method);
    }
}
