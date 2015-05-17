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


Receiver::Receiver(QObject* parent_object, const std::function<void()>& target): QObject(parent_object), m_target(target)
{

}


void Receiver::notification()
{
    m_target();
}


SignalFilter::SignalFilter(QObject* parent_object): QObject(parent_object)
{

}


SignalFilter::~SignalFilter()
{

}


void SignalFilter::connect(QObject* sender_obj, const char* signal, const std::function<void()>& target, Qt::ConnectionType type)
{
    Receiver* rec = new Receiver(this, target);
    QObject::connect(sender_obj, signal, rec, SLOT(notification()), type);
}
