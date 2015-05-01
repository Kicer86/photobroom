/*
 * Generator for info and hints.
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

#ifndef INFOGENERATOR_HPP
#define INFOGENERATOR_HPP

#include <QObject>

class SignalFilter;

namespace Database
{
    struct IDatabase;
}

class InfoGenerator: public QObject
{
        Q_OBJECT

    public:
        InfoGenerator(QObject *);
        InfoGenerator(const InfoGenerator &) = delete;
        ~InfoGenerator();
        InfoGenerator& operator=(const InfoGenerator &) = delete;

        void set(Database::IDatabase *);

    private:
        Database::IDatabase* m_database;
        SignalFilter* m_signalFiler;

    private slots:
        void dbChanged();
};

#endif // INFOGENERATOR_HPP
