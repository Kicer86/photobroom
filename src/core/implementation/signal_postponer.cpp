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

using namespace std::chrono;


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
