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

#ifndef SIGNALFILTER_HPP
#define SIGNALFILTER_HPP

#include <chrono>
#include <functional>

#include <QObject>


class Receiver: public QObject
{
        Q_OBJECT

    public:
        Receiver(QObject *, const std::function<void()> &, const std::chrono::milliseconds &);

    private:
        const std::chrono::milliseconds m_block_time;
        std::function<void()> m_target;
        bool m_blocked;
        bool m_dirty;

    public slots:
        void notification();
        void clear();
};

class SignalFilter: public QObject
{
    public:
        SignalFilter(QObject *);
        SignalFilter(const SignalFilter &) = delete;
        ~SignalFilter();

        SignalFilter& operator=(const SignalFilter &) = delete;

        void connect(QObject* sender_obj, const char* signal, const std::function<void()>& target,
                     const std::chrono::milliseconds &, Qt::ConnectionType type = Qt::AutoConnection);
};

#endif // SIGNALFILTER_HPP
