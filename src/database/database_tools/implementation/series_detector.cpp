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

#include "../series_detector.hpp"

#include <QDateTime>

#include <core/iexif_reader.hpp>
#include <ibackend.hpp>
#include <iphoto_operator.hpp>


namespace
{
    qint64 timestamp(const Photo::Data& data)
    {
        qint64 timestamp = -1;

        const auto dateIt = data.tags.find(TagTypes::Date);

        if (dateIt != data.tags.end())
        {
            const QDate date = dateIt->second.getDate();
            const auto timeIt = data.tags.find(TagTypes::Time);
            const QTime time = timeIt != data.tags.end()? timeIt->second.getTime(): QTime();
            const QDateTime dateTime(date, time);

            timestamp = dateTime.toMSecsSinceEpoch();
        }

        return timestamp;
    }
}


SeriesDetector::SeriesDetector(Database::IBackend* backend, IExifReader* exif):
    m_backend(backend), m_exifReader(exif)
{

}


std::vector<SeriesDetector::GroupCandidate> SeriesDetector::listCandidates() const
{
    std::vector<GroupCandidate> result;

    // find photos which are not part of any group
    Database::IFilter::Ptr group_filter = std::make_unique<Database::FilterPhotosWithRole>(Database::FilterPhotosWithRole::Role::Regular);
    const auto photos = m_backend->photoOperator().getPhotos( {group_filter} );

    // collect photos with SequenceNumber and timestamp in exif
    std::multiset<PhotosWithSequence> sequences_by_time = analyze_photos(photos);
    auto sequence_groups = split_into_groups(sequences_by_time);
    determine_type(sequence_groups);

    result = sequence_groups;

    return result;
}


const std::multiset<SeriesDetector::PhotosWithSequence> SeriesDetector::analyze_photos(const std::vector<Photo::Id>& photos) const
{
    std::multiset<PhotosWithSequence> sequences_by_time;

    for (const Photo::Id& id: photos)
    {
        const Photo::Data data = m_backend->getPhoto(id);
        const std::optional<std::any> seq = m_exifReader->get(data.path, IExifReader::TagType::SequenceNumber);

        if (seq)
        {
            const std::any& rawData = *seq;
            const int seqNum = std::any_cast<int>(rawData);
            const qint64 time = timestamp(data);

            if (time != -1)
            {
                Photo::DataDelta delta(id);
                delta.insert<Photo::Field::Path>(data.path);
                sequences_by_time.emplace(time, seqNum, delta);
            }
        }
    }

    return sequences_by_time;
}


std::vector<SeriesDetector::GroupCandidate> SeriesDetector::split_into_groups(const std::multiset<PhotosWithSequence>& data) const
{
    const int initial_sequence_value = 1;
    int expected_seq = initial_sequence_value;

    std::vector<Photo::DataDelta> group;
    std::vector<GroupCandidate> results;

    auto dumpGroup = [&results, &group]()
    {
        GroupCandidate detection;
        detection.type = Group::Type::Invalid;
        detection.members = group;

        results.push_back(detection);

        group.clear();
    };

    for(auto it = data.cbegin(); it != data.cend(); ++it)
    {
        const int& seqNum = it->sequence;
        const Photo::DataDelta& ph_data = it->data;

        if (seqNum != expected_seq)     // sequenceNumber does not match expectations? finish/skip group
        {
            if (group.empty() == false)
                dumpGroup();

            expected_seq = initial_sequence_value;
        }

        if (seqNum == expected_seq)     // sequenceNumber matches expectations? begin/continue group
        {
            group.push_back(ph_data);
            expected_seq++;
        }

        if (std::next(it) == data.cend())
            dumpGroup();
    }

    return results;
}


void SeriesDetector::determine_type(std::vector<GroupCandidate>& sequence_groups) const
{
    for(SeriesDetector::GroupCandidate& group: sequence_groups)
    {
        std::vector<float> exposures;

        for(const Photo::DataDelta& member: group.members)
        {
            const QString& path = member.get<Photo::Field::Path>();
            const std::optional<std::any> exposureRaw = m_exifReader->get(path, IExifReader::TagType::Exposure);
            const float exposure = exposureRaw.has_value()? std::any_cast<float>(*exposureRaw): 0.0f;

            exposures.push_back(exposure);
        }

        const bool constant_exposure = std::equal(exposures.cbegin() + 1, exposures.cend(), exposures.cbegin());

        group.type = constant_exposure? Group::Type::Animation: Group::Type::HDR;
    }
}
