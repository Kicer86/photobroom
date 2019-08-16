/*
 * Tool for series detection
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef SERIESDETECTOR_HPP
#define SERIESDETECTOR_HPP

#include <set>

#include <database/group.hpp>
#include <database/photo_types.hpp>
#include <database_export.h>


namespace Database
{
    struct IBackend;
}

struct IExifReader;


class DATABASE_EXPORT SeriesDetector
{
    public:
        struct GroupCandidate
        {
            Group::Type type;
            std::vector<Photo::Id> members;
        };

        SeriesDetector(Database::IBackend *, IExifReader *);

        std::vector<GroupCandidate> listDetections() const;

    private:
        Database::IBackend* m_backend;
        IExifReader* m_exifReader;

        std::vector<GroupCandidate> split_into_groups(const std::multiset<std::tuple<qint64, int, float, Photo::Id>> &) const;
};

#endif // SERIESDETECTOR_HPP
