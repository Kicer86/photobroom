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

#ifndef SIGNALFILTER_HPP
#define SIGNALFILTER_HPP

#include <QObject>

class QSignalMapper;


class SignalFilter: public QObject
{
        Q_OBJECT

    public:
        SignalFilter(QObject *);
        SignalFilter(const SignalFilter &) = delete;
        ~SignalFilter();

        SignalFilter& operator=(const SignalFilter &) = delete;

        void connect(QObject* sender, const char* signal,
                     QObject* receiver, const char* method, Qt::ConnectionType type = Qt::AutoConnection);


    private:
        struct Receiver
        {
            QObject* receiver;
            const char* method;
        };

        std::map<const QObject *, Receiver> m_signals;
        QSignalMapper* m_mapper;

    private slots:
        void notification(QObject *);
};

#endif // SIGNALFILTER_HPP
