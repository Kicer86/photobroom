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


#include <unordered_set>
#include <QDateTime>

#include <core/iexif_reader.hpp>
#include <core/media_types.hpp>
#include <core/task_executor_utils.hpp>
#include <core/tags_utils.hpp>
#include <ibackend.hpp>
#include <iphoto_operator.hpp>

#include "database_executor_traits.hpp"
#include "../series_detector.hpp"
#include "photo_utils.hpp"

namespace
{

    int readExposure(const std::any& exposure)
    {
        const float exp = std::any_cast<float>(exposure);

        return static_cast<int>(exp * 100);   // convert original exposure to centi-exposure
    }

    class IGroupValidator
    {
    public:
        virtual ~IGroupValidator() = default;
        virtual void setCurrentPhoto(const Photo::DataDelta &) = 0;
        virtual bool canBePartOfGroup() const = 0;
        virtual void accept() = 0;
        virtual void reset() = 0;
        virtual GroupCandidate::Type type() const = 0;
    };

    class GroupValidator_ExifBasedSeries: public IGroupValidator
    {
    public:
        GroupValidator_ExifBasedSeries(IExifReader& exif, const SeriesDetector::Rules &)
            : m_exifReader(exif)
        {

        }

        void setCurrentPhoto(const Photo::DataDelta& d) override
        {
            m_sequence = m_exifReader.get(d.get<Photo::Field::Path>(), IExifReader::TagType::SequenceNumber);
        }

        bool canBePartOfGroup() const override
        {
            const bool has_exif_data = m_sequence.has_value();

            if (has_exif_data)
            {
                const int s = std::any_cast<int>(m_sequence.value());
                auto s_it = m_sequence_numbers.find(s);

                return s_it == m_sequence_numbers.end();
            }
            else
                return false;
        }

        void accept() override
        {
            assert(m_sequence);

            const int s = std::any_cast<int>(m_sequence.value());

            m_sequence_numbers.insert(s);
        }

        void reset() override
        {
            m_sequence.reset();
            m_sequence_numbers.clear();
        }

        GroupCandidate::Type type() const override
        {
            return GroupCandidate::Type::Series;
        }

        std::optional<std::any> m_sequence;
        std::unordered_set<int> m_sequence_numbers;
        IExifReader& m_exifReader;
    };


    class GroupValidator_FileNameBasedSeries: public IGroupValidator
    {
    public:
        GroupValidator_FileNameBasedSeries()
        {

        }

        void setCurrentPhoto(const Photo::DataDelta& d) override
        {
            const auto path = d.get<Photo::Field::Path>();
            const int burstLen = 5;
            const int burstIdx = path.indexOf("BURST");

            if (burstIdx > -1)
            {
                const int startIdx = burstIdx + burstLen;
                int endIdx = startIdx;

                for (; endIdx < path.size(); endIdx++)
                    if (path[endIdx].isNumber() == false)
                        break;

                const auto idxStr = path.mid(startIdx, endIdx - startIdx);
                m_index = idxStr.toInt();
            }
        }

        bool canBePartOfGroup() const override
        {
            if (m_index > -1)
            {
                auto s_it = m_burst_numbers.find(m_index);

                return s_it == m_burst_numbers.end();
            }
            else
                return false;
        }

        void accept() override
        {
            m_burst_numbers.insert(m_index);
        }

        void reset() override
        {
            m_index = -1;
            m_burst_numbers.clear();
        }

        GroupCandidate::Type type() const override
        {
            return GroupCandidate::Type::Series;
        }

        std::unordered_set<int> m_burst_numbers;
        int m_index = -1;
    };


    class GroupValidator_HDR: public GroupValidator_ExifBasedSeries
    {
        typedef GroupValidator_ExifBasedSeries Base;

    public:
        GroupValidator_HDR(IExifReader& exif, const SeriesDetector::Rules& r)
            : Base(exif, r)
        {

        }

        void setCurrentPhoto(const Photo::DataDelta& d) override
        {
            Base::setCurrentPhoto(d);
            m_exposure = m_exifReader.get(d.get<Photo::Field::Path>(), IExifReader::TagType::Exposure);
        }

        bool canBePartOfGroup() const override
        {
            const bool has_exif_data = Base::canBePartOfGroup() && m_exposure;

            if (has_exif_data)
            {
                const int e = readExposure(m_exposure.value());

                auto e_it = m_exposures.find(e);

                return e_it == m_exposures.end();
            }
            else
                return false;
        }

        void accept() override
        {
            assert(m_exposure);

            Base::accept();

            const int e = readExposure(m_exposure.value());
            m_exposures.insert(e);
        }

        void reset() override
        {
            Base::reset();
            m_exposure.reset();
            m_exposures.clear();
        }

        GroupCandidate::Type type() const override
        {
            return GroupCandidate::Type::HDR;
        }

        std::optional<std::any> m_exposure;
        std::unordered_set<int> m_exposures;
    };

    class GroupValidator_Generic: public IGroupValidator
    {
    public:
        explicit GroupValidator_Generic(const SeriesDetector::Rules& r)
            : m_prev_stamp(0)
            , m_rules(r)
        {

        }

        void setCurrentPhoto(const Photo::DataDelta& d) override
        {
            m_current_stamp = Tag::timestamp(d.get<Photo::Field::Tags>());
        }

        bool canBePartOfGroup() const override
        {
            return m_prev_stamp.count() == 0 || m_current_stamp - m_prev_stamp <= m_rules.manualSeriesMaxGap;
        }

        void accept() override
        {
            m_prev_stamp = m_current_stamp;
        }

        void reset() override
        {
            m_prev_stamp = std::chrono::milliseconds(0);
        }

        GroupCandidate::Type type() const override
        {
            return GroupCandidate::Type::Generic;
        }

        std::chrono::milliseconds m_prev_stamp,
                                  m_current_stamp;
        const SeriesDetector::Rules& m_rules;
    };

    class SeriesExtractor
    {
    public:
        SeriesExtractor(Database::IDatabase& db,
                        const std::deque<Photo::DataDelta>& photos,
                        std::unique_ptr<ILogger> logger,
                        const QPromise<std::vector<GroupCandidate>>* p)
            : m_db(db)
            , m_photos(photos)
            , m_logger(std::move(logger))
            , m_promise(p)
        {

        }

        std::vector<GroupCandidate> extract( IGroupValidator& validator)
        {
            std::vector<GroupCandidate> results;

            for (auto it = m_photos.begin(); it != m_photos.end();)
            {
                if (m_promise && m_promise->isCanceled())
                    throw abort_exception();

                validator.reset();

                std::vector<Photo::Id> members;

                for (auto it2 = it; it2 != m_photos.end(); ++it2)
                {
                    const Photo::DataDelta& data = *it2;

                    validator.setCurrentPhoto(data);

                    if (validator.canBePartOfGroup())
                    {
                        members.push_back(data.getId());
                        validator.accept();
                    }
                    else
                        break;
                }

                const auto membersCount = members.size();

                if (membersCount > 1)
                {
                    GroupCandidate group;
                    group.type = validator.type();

                    QStringList ids;
                    std::transform(members.begin(), members.end(), std::back_inserter(ids), [](const auto& id){ return QString::number(id.value()); });
                    m_logger->trace(QString("Detected series of %1 photos: %2").arg(membersCount).arg(ids.join(", ")));

                    // Id to Data  TODO: this can be done in background
                    std::transform(members.begin(), members.end(), std::back_inserter(group.members), [this](const Photo::Id& id)
                    {
                        return evaluate<Photo::DataDelta(Database::IBackend &)>(m_db, [id](Database::IBackend& backend)
                        {
                            return backend.getPhotoDelta(id, {Photo::Field::Path, Photo::Field::Flags, Photo::Field::GroupInfo});
                        });
                    });

                    results.push_back(group);

                    auto first = it;
                    auto last = first + membersCount;

                    it = m_photos.erase(first, last);
                }
                else
                    ++it;
            }

            return results;
        }

    private:
        Database::IDatabase& m_db;
        std::deque<Photo::DataDelta> m_photos;
        std::unique_ptr<ILogger> m_logger;
        const QPromise<std::vector<GroupCandidate>>* m_promise;
    };
}


SeriesDetector::Rules::Rules(std::chrono::milliseconds gap, bool neighbours)
    : manualSeriesMaxGap(gap)
    , detectTimeNeighbors(neighbours)
{

}


SeriesDetector::SeriesDetector(ILogger& logger, Database::IDatabase& db, IExifReader& exif, const QPromise<std::vector<GroupCandidate>>* p)
    : m_logger(logger)
    , m_db(db)
    , m_promise(p)
    , m_exifReader(exif)
{

}


std::vector<GroupCandidate> SeriesDetector::listCandidates(const Rules& rules) const
{
    QElapsedTimer timer;
    timer.start();

    const std::deque<Photo::DataDelta> candidates =
        evaluate<std::deque<Photo::DataDelta>(Database::IBackend &)>(m_db, [](Database::IBackend& backend)
    {
        std::vector<GroupCandidate> result;

        // find photos which are not part of any group
        const Database::FilterPhotosWithRole group_filter(Database::FilterPhotosWithRole::Role::Regular);

        // and are valid
        const auto valid_photos_filter = Database::getValidPhotosFilter();

        // photos - candidates for series/groups
        const auto photos = backend.photoOperator().onPhotos( Database::GroupFilter{group_filter, valid_photos_filter}, Database::Actions::Sort(Database::Actions::Sort::By::Timestamp) );

        std::deque<Photo::DataDelta> deltas;
        std::ranges::transform(photos, std::back_inserter(deltas), [&backend](const Photo::Id& id)
        {
            return backend.getPhotoDelta(id, {Photo::Field::Tags, Photo::Field::Path});
        });

        return deltas;
    });

    const QString log = QString("Collecting photos for grouping took %1s").arg(timer.elapsed() / 1000);

    m_logger.debug(log);

    return analyzePhotos(candidates, rules);
}


std::vector<GroupCandidate> SeriesDetector::analyzePhotos(const std::deque<Photo::DataDelta>& photos, const Rules& rules) const
{
    QElapsedTimer timer;
    std::deque<Photo::DataDelta> suitablePhotos;

    // grouping works for images only
    timer.start();
    std::ranges::copy_if(photos, std::back_inserter(suitablePhotos), [](const auto& photo) {
        return MediaTypes::isImageFile(Photo::getPath(photo));
    });

    m_logger.debug(QString("Validating media type took %1s").arg(timer.elapsed() / 1000));

    // drop images which were not made in similar time
    timer.restart();
    std::deque<Photo::DataDelta> prefiltered = removeSingles(suitablePhotos, rules);

    m_logger.debug(QString("Prefiltration time: %1s. %2 photos left.")
        .arg(timer.elapsed() / 1000)
        .arg(prefiltered.size()));

    try
    {
        SeriesExtractor extractor(m_db, prefiltered, m_logger.subLogger("SeriesExtractor"), m_promise);
        std::vector<GroupCandidate> sequences;

        timer.restart();
        GroupValidator_HDR hdrValidator(m_exifReader, rules);
        const auto hdrs = extractor.extract(hdrValidator);
        std::ranges::copy(hdrs, std::back_inserter(sequences));
        m_logger.debug(QString("HDRs extraction took: %1s").arg(timer.elapsed() / 1000));

        timer.restart();
        GroupValidator_ExifBasedSeries animationsValidator(m_exifReader, rules);
        const auto exifSeries = extractor.extract(animationsValidator);
        std::ranges::copy(exifSeries, std::back_inserter(sequences));
        m_logger.debug(QString("Exif based series extraction took: %1s").arg(timer.elapsed() / 1000));

        timer.restart();
        GroupValidator_FileNameBasedSeries fileNameValidator;
        const auto fileNameSeries = extractor.extract(fileNameValidator);
        std::ranges::copy(fileNameSeries, std::back_inserter(sequences));
        m_logger.debug(QString("Filename based series extraction took: %1s").arg(timer.elapsed() / 1000));

        if (rules.detectTimeNeighbors)
        {
            timer.restart();
            GroupValidator_Generic genericValidator(rules);
            const auto generics = extractor.extract(genericValidator);
            std::ranges::copy(generics, std::back_inserter(sequences));
            m_logger.debug(QString("Generic groups extraction took: %1s").arg(timer.elapsed() / 1000));
        }

        return sequences;
    }
    catch (const abort_exception &)
    {
        return {};
    }
}


std::deque<Photo::DataDelta> SeriesDetector::removeSingles(const std::deque<Photo::DataDelta>& suitablePhotos, const Rules& rules) const
{
    std::deque<Photo::DataDelta> prefiltered;

    for(std::size_t i = 0; i < suitablePhotos.size(); i++)
    {
        std::vector<std::chrono::milliseconds> neighbours;

        const auto currentTimestamp = Tag::timestamp(suitablePhotos[i].get<Photo::Field::Tags>());

        if (currentTimestamp == std::chrono::milliseconds(0))
            continue;

        if (i > 0)
        {
            const auto previousTimestamp = Tag::timestamp(suitablePhotos[i - 1].get<Photo::Field::Tags>());
            neighbours.push_back(previousTimestamp);
        }

        if (i < suitablePhotos.size() - 1)
        {
            const auto nextTimestamp = Tag::timestamp(suitablePhotos[i + 1].get<Photo::Field::Tags>());
            neighbours.push_back(nextTimestamp);
        }

        const bool any = std::ranges::any_of(neighbours, [&currentTimestamp, rules](const auto& neighbourTimestamp)
        {
            return std::chrono::abs(currentTimestamp - neighbourTimestamp) <= rules.manualSeriesMaxGap;
        });

        if (any)
            prefiltered.push_back(suitablePhotos[i]);
    }

    return prefiltered;
}
