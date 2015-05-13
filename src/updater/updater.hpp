/*
 * Auto updater
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

#ifndef UPDATER_HPP
#define UPDATER_HPP

#include <memory>

#include <QObject>

#include "updater_export.h"

class UpdaterImpl;


class UPDATER_EXPORT Updater: public QObject
{
        Q_OBJECT

    public:
        Updater();
        Updater(const Updater &) = delete;
        ~Updater();

        Updater& operator=(const Updater &) = delete;

    private:
        std::unique_ptr<UpdaterImpl> m_impl;

    signals:
        void currentVersion(const QString &);    //returns url to newest version or empty string when it's up to date
};

#endif // UPDATER_H
