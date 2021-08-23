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

#ifndef SIGNALMAPPER_HPP
#define SIGNALMAPPER_HPP

#include <QObject>

#include <database/iphoto_info.hpp>

#include "database_export.h"


namespace Database
{
    struct IDatabase;

    class DATABASE_EXPORT SignalMapper final: public QObject
    {
        Q_OBJECT

        public:
            SignalMapper(IDatabase* = nullptr);
            ~SignalMapper() = default;

            void set(IDatabase *);

        signals:
            void photoModified(const IPhotoInfo::Ptr &) const;                    // emited when photo updated

        private:
            mutable std::mutex m_dbMutex;
            IDatabase* m_db;

            void i_photosModified(const std::set<Photo::Id> &) const;
    };
}

#endif // SIGNALMAPPER_HPP
