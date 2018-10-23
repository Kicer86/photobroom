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

#include <functional>

#include <QObject>


struct PatientPolicy
{
    bool operator()(const std::chrono::steady_clock::time_point& last,
                    const std::chrono::steady_clock::time_point& current) const
    {
        if (last.time_since_epoch() != 0)
        {
        }

        return true;
    }
};


template<typename Signal2, typename PostponePolicy>
class SignalPostponer: public QObject
{
    public:
        template<typename Fire>
        SignalPostponer(Fire fire, QObject* p, PostponePolicy policy):
            QObject(p),
            m_fire(fire),
            m_policy(policy)
        {
            reset();
        }

        void notify()
        {
            auto current_time = std::chrono::steady_clock::now();

            const bool fire = m_policy(m_last, current_time);

            if (fire)
                m_fire();
            else
                m_last = current_time;
        }

    private:
        std::function<void()> m_fire;
        PostponePolicy m_policy;
        std::chrono::steady_clock::time_point m_last;

        void reset()
        {
            m_last = 0;
        }
};


template<typename Signal1, typename Dst, typename Signal2, typename PostponePolicy>
void lazy_connect(QObject* src, Signal1 sig1, Dst* dst, Signal2 sig2, PostponePolicy policy)
{
    typedef SignalPostponer<Signal2, PostponePolicy> Postponer;
    Postponer* postponer = new Postponer([&dst, &sig2](){dst->sig2();}, src, policy);

    QObject::connect(src, sig1, postponer, &Postponer::notify);
}

#endif // SIGNALPOSTPONER_HPP
