/*
 * Photo Broom - photos management tool.
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

#include <chrono>
#include <deque>
#include <QPromise>

#include <core/ilogger.hpp>
#include <database/group.hpp>
#include <database/idatabase.hpp>
#include <database/photo_data.hpp>
#include <database_export.h>

#include "series_candidate.hpp"


struct IExifReader;


class DATABASE_EXPORT SeriesDetector
{
    public:
        using ExplicitDelta = Photo::ExplicitDelta<Photo::Field::Tags, Photo::Field::Path>;

        struct DATABASE_EXPORT Rules
        {
            std::chrono::milliseconds manualSeriesMaxGap;
            bool detectTimeNeighbors;

            Rules(std::chrono::milliseconds manualSeriesMaxGap = std::chrono::seconds(10), bool neighbors = false);
        };

        SeriesDetector(ILogger &, Database::IDatabase &, IExifReader &, QPromise<std::vector<GroupCandidate>> &);

        std::vector<GroupCandidate> listCandidates(const Rules& = Rules()) const;

    private:
        ILogger& m_logger;
        Database::IDatabase& m_db;
        QPromise<std::vector<GroupCandidate>>& m_promise;
        IExifReader& m_exifReader;

        std::vector<GroupCandidate> analyzePhotos(const std::deque<ExplicitDelta> &, const Rules &) const;
        std::deque<ExplicitDelta> removeSingles(const std::deque<ExplicitDelta> &, const Rules &) const;
};

#endif
