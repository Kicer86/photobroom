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

#include <unordered_set>
#include <QDateTime>

#include <core/iexif_reader.hpp>
#include <ibackend.hpp>
#include <iphoto_operator.hpp>


namespace
{
    std::chrono::milliseconds timestamp(const Photo::Data& data)
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

        return std::chrono::milliseconds(timestamp);
    }

    template<Group::Type>
    class GroupValidator;

    template<>
    class GroupValidator<Group::Type::HDR>
    {
    public:
        GroupValidator(IExifReader& exif)
            : m_exifReader(exif)
        {

        }

        void setCurrentPhoto(const Photo::Data& d)
        {
            data = d;
            sequence = m_exifReader.get(data.path, IExifReader::TagType::SequenceNumber);
            exposure = m_exifReader.get(data.path, IExifReader::TagType::Exposure);
        }

        bool canBePartOfGroup()
        {
            const bool has_exif_data = sequence && exposure;

            if (has_exif_data)
            {
                const int s = std::any_cast<int>(sequence.value());
                const int e = std::any_cast<float>(exposure.value());

                auto s_it = sequence_numbers.find(s);
                auto e_it = exposures.find(e);

                return s_it == sequence_numbers.end() &&
                       e_it == exposures.end();
            }
            else
                return false;
        }

        void accept()
        {
            assert(sequence);
            assert(exposure);

            const int s = std::any_cast<int>(sequence.value());
            const int e = std::any_cast<float>(exposure.value());

            sequence_numbers.insert(s);
            exposures.insert(e);
        }

        Photo::Data data;
        std::optional<std::any> sequence;
        std::optional<std::any> exposure;

        std::unordered_set<int> sequence_numbers;
        std::unordered_set<float> exposures;
        IExifReader& m_exifReader;
    };

    template<>
    class GroupValidator<Group::Type::Animation>
    {
    public:
        GroupValidator(IExifReader& exif)
            : m_exifReader(exif)
        {

        }

        void setCurrentPhoto(const Photo::Data& d)
        {
            data = d;
            sequence = m_exifReader.get(data.path, IExifReader::TagType::SequenceNumber);
        }

        bool canBePartOfGroup()
        {
            const bool has_exif_data = sequence.has_value();

            if (has_exif_data)
            {
                const int s = std::any_cast<int>(sequence.value());

                auto s_it = sequence_numbers.find(s);

                return s_it == sequence_numbers.end();
            }
            else
                return false;
        }

        void accept()
        {
            assert(sequence);

            const int s = std::any_cast<int>(sequence.value());

            sequence_numbers.insert(s);
        }

        Photo::Data data;
        std::optional<std::any> sequence;

        std::unordered_set<int> sequence_numbers;
        IExifReader& m_exifReader;
    };

    template<>
    class GroupValidator<Group::Type::Generic>
    {
    };

    class SeriesTaker
    {
    public:
        SeriesTaker(Database::IBackend& backend, IExifReader* exifReader)
            : m_backend(backend)
            , m_exifReader(exifReader)
        {

        }

        template<Group::Type type>
        std::vector<SeriesDetector::GroupCandidate> take(std::deque<Photo::Id>& photos)
        {
            std::vector<SeriesDetector::GroupCandidate> results;

            for (auto it = photos.begin(); it != photos.end();)
            {
                SeriesDetector::GroupCandidate group;
                group.type = type;

                GroupValidator<type> validator(*m_exifReader);

                for (auto it2 = it; it2 != photos.end(); ++it2)
                {
                    const auto id = *it2;
                    const Photo::Data data = m_backend.getPhoto(id);

                    validator.setCurrentPhoto(data);

                    if (validator.canBePartOfGroup())
                    {
                        group.members.push_back(data);
                        validator.accept();
                    }
                    else
                        break;
                }

                const auto members = group.members.size();

                // each photo should have different exposure
                if (members > 1)
                {
                    results.push_back(group);

                    auto first = it;
                    auto last = first + members;

                    it = photos.erase(first, last);
                }
                else
                    ++it;
            }

            return results;
        }

    private:
        Database::IBackend& m_backend;
        IExifReader* m_exifReader;
    };
}


SeriesDetector::Rules::Rules(std::chrono::milliseconds manualSeriesMaxGap)
    : manualSeriesMaxGap(manualSeriesMaxGap)
{

}


SeriesDetector::SeriesDetector(Database::IBackend& backend, IExifReader* exif):
    m_backend(backend), m_exifReader(exif)
{

}


std::vector<SeriesDetector::GroupCandidate> SeriesDetector::listCandidates(const Rules& rules) const
{
    std::vector<GroupCandidate> result;

    // find photos which are not part of any group
    Database::IFilter::Ptr group_filter = std::make_unique<Database::FilterPhotosWithRole>(Database::FilterPhotosWithRole::Role::Regular);
    const auto photos = m_backend.photoOperator().onPhotos( {group_filter}, Database::Actions::SortByTimestamp() );

    // find groups
    auto sequence_groups = analyze_photos(photos, rules);

    return sequence_groups;
}


std::vector<SeriesDetector::GroupCandidate> SeriesDetector::take_hdr(std::deque<Photo::Id>& photos) const
{
    SeriesTaker t(m_backend, m_exifReader);
    return t.take<Group::Type::HDR>(photos);
}


std::vector<SeriesDetector::GroupCandidate> SeriesDetector::take_animations(std::deque<Photo::Id>& photos) const
{
    SeriesTaker t(m_backend, m_exifReader);
    return t.take<Group::Type::Animation>(photos);
}


std::vector<SeriesDetector::GroupCandidate> SeriesDetector::take_close(std::deque<Photo::Id>& photos, const Rules& rules) const
{
    std::vector<GroupCandidate> results;

    for (auto it = photos.begin(); it != photos.end();)
    {
        GroupCandidate group;
        group.type = Group::Type::Generic;
        std::chrono::milliseconds prev_stamp;

        for (auto it2 = it; it2 != photos.end(); ++it2)
        {
            const auto id = *it2;
            const Photo::Data data = m_backend.getPhoto(id);
            const auto current_stamp = timestamp(data);

            if (group.members.empty() || current_stamp - prev_stamp <= rules.manualSeriesMaxGap)
            {
                group.members.push_back(data);
                prev_stamp = current_stamp;
            }
            else
                break;
        }

        const auto members = group.members.size();

        if (members > 1)
        {
            results.push_back(group);

            auto first = it;
            auto last = first + members;

            it = photos.erase(first, last);
        }
        else
            ++it;
    }

    return results;
}


std::vector<SeriesDetector::GroupCandidate> SeriesDetector::analyze_photos(const std::vector<Photo::Id>& photos,
                                                                           const Rules& rules) const
{
    std::deque<Photo::Id> photos_deq(photos.begin(), photos.end());

    auto hdrs = take_hdr(photos_deq);
    auto animations = take_animations(photos_deq);
    auto generics = take_close(photos_deq, rules);

    std::vector<SeriesDetector::GroupCandidate> sequences;
    std::copy(hdrs.begin(), hdrs.end(), std::back_inserter(sequences));
    std::copy(animations.begin(), animations.end(), std::back_inserter(sequences));
    std::copy(generics.begin(), generics.end(), std::back_inserter(sequences));

    return sequences;
}
