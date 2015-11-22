/*
 * Utility for checking if path already exists in database
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

#ifndef PATHCHECKER_HPP
#define PATHCHECKER_HPP

#include <QObject>

#include <core/callback_ptr.hpp>
#include <database/iphoto_info.hpp>

class QString;

namespace Database
{
    struct IDatabase;
}

class PathChecker: public QObject
{
        Q_OBJECT

    public:
        PathChecker();
        PathChecker(const PathChecker &) = delete;
        ~PathChecker();

        void set(Database::IDatabase *);

        PathChecker& operator=(const PathChecker &) = delete;

        void checkFile(const QString &);

    private:
        std::map<QString, bool> m_cache;
        Database::IDatabase* m_database;
        safe_callback_ctrl m_callbackCtrl;

        void gotPhotos(const IPhotoInfo::List &);

    signals:
        void fileChecked(const QString &, bool);
};

#endif // PATHCHECKER_HPP
