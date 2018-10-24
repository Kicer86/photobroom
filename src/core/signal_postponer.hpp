/*
 * Tool for postponing (accumulating) noisy signals
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
#include <optional>

#include <QObject>
#include <QTimer>


template<typename Signal2>
class SignalPostponer: public QObject
{
    public:
        template<typename Fire>
        SignalPostponer(Fire fire, QObject* p):
            QObject(p)
        {
            connect(&m_timer, &QTimer::timeout, fire);
            m_timer.setSingleShot(true);
        }

        void notify()
        {
            using namespace std::chrono;

            m_timer.start(250ms);
        }

    private:
        QTimer m_timer;
};


template<typename Signal1, typename Dst, typename Signal2>
void lazy_connect(QObject* src, Signal1 sig1, Dst* dst, Signal2 slot1)
{
    typedef SignalPostponer<Signal2> Postponer;

    Postponer* postponer = new Postponer([&dst, &slot1]()
        {
            // launch destination slot
            dst->slot1();
        },
        src);

    QObject::connect(src, sig1, postponer, &Postponer::notify);
}

#endif // SIGNALPOSTPONER_HPP
