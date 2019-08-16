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
#include <database/photo_data.hpp>
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
            std::vector<Photo::DataDelta> members;
        };

        SeriesDetector(Database::IBackend *, IExifReader *);

        std::vector<GroupCandidate> listCandidates() const;

    private:
        struct PhotosWithSequence
        {
            PhotosWithSequence(qint64 t, int s, const Photo::DataDelta& d):
                timestamp(t),
                sequence(s),
                data(d)
            {

            }

            bool operator<(const PhotosWithSequence& other) const
            {
                return std::tie(timestamp, sequence, data) <
                       std::tie(other.timestamp, other.sequence, other.data);
            }

            qint64 timestamp;
            int sequence;
            Photo::DataDelta data;
        };

        Database::IBackend* m_backend;
        IExifReader* m_exifReader;

        std::vector<GroupCandidate> split_into_groups(const std::multiset<PhotosWithSequence> &) const;
        void determine_type(std::vector<GroupCandidate> &) const;
};

#endif // SERIESDETECTOR_HPP
