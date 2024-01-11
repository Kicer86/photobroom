/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "aexif_reader.hpp"

#include <QByteArray>
#include <QDate>
#include <QFileInfo>
#include <QStringList>
#include <QTime>
#include <stdexcept>

#include "tag.hpp"
#include "base_tags.hpp"


template<typename T>
std::optional<std::any> exiv_result(const std::optional<T>& value)
{
    std::optional<std::any> result;

    if (value.has_value())
        result = *value;

    return result;
}



AExifReader::AExifReader(): m_id(std::this_thread::get_id())
{

}


AExifReader::~AExifReader()
{

}


Tag::TagsList AExifReader::getTagsFor(const QString& path)
{
    assert(m_id == std::this_thread::get_id());

    const QFileInfo fileInfo(path);
    const QString full_path = fileInfo.absoluteFilePath();

    collect(full_path);

    const Tag::TagsList tagData = feedDateAndTime();

    return tagData;
}


std::optional<std::any> AExifReader::get(const QString& path, const IExifReader::TagType& type)
{
    assert(m_id == std::this_thread::get_id());

    const QFileInfo fileInfo(path);
    const QString full_path = fileInfo.absoluteFilePath();

    collect(full_path);

    std::optional<std::any> result;

    switch(type)
    {
        case TagType::SequenceNumber:
            result = exiv_result(readInt(TagType::SequenceNumber));
            break;

        case TagType::Orientation:
            result = exiv_result(readInt(TagType::Orientation));
            break;

        case TagType::DateTimeOriginal:
            result = exiv_result(readString(TagType::DateTimeOriginal));
            break;

        case TagType::PixelXDimension:
            result = exiv_result(readLong(TagType::PixelXDimension));
            break;

        case TagType::PixelYDimension:
            result = exiv_result(readLong(TagType::PixelYDimension));
            break;

        case TagType::Exposure:
            result = exiv_result(readRational(TagType::Exposure));
            break;

        case TagType::Projection:
            result = exiv_result(readString(TagType::Projection));
            break;
    }

    return result;
}


Tag::TagsList AExifReader::feedDateAndTime() const
{
    Tag::TagsList tagData;

    const std::optional<std::string> dateTimeOrirignal = read(TagType::DateTimeOriginal);

    if (dateTimeOrirignal.has_value())
    {
        const QString v(dateTimeOrirignal->c_str());
        const QStringList time_splitted = v.split(" ");

        if (time_splitted.size() == 2)
        {
            QString date_raw = time_splitted[0];
            const QString time_raw = time_splitted[1];

            const QDate date = QDate::fromString(date_raw, "yyyy:MM:dd");
            const QTime time = QTime::fromString(time_raw, "hh:mm:ss");

            if (date.isValid())
                tagData[Tag::Types::Date] = TagValue(date);

            if (time.isValid())
                tagData[Tag::Types::Time] = TagValue(time);
        }
    }

    return tagData;
}


std::optional<int> AExifReader::readInt(const TagType& tagType) const
{
    std::optional<int> result;
    const std::optional<std::string> valueRaw = read(tagType);

    if (valueRaw.has_value())
    {
        try
        {
            const int value = stoi(*valueRaw);

            if (value > 0)
                result = value;
        }
        catch(const std::invalid_argument &) {}
        catch(const std::out_of_range &) {}
    }

    return result;
}


std::optional<std::string> AExifReader::readString(const TagType& tagType) const
{
    const std::optional<std::string> result = read(tagType);

    return result;
}


std::optional<long> AExifReader::readLong(const IExifReader::TagType& tagType) const
{
    std::optional<long> result;
    const std::optional<std::string> valueRaw = read(tagType);

    if (valueRaw.has_value())
    {
        try
        {
            const long value = stol(*valueRaw);

            if (value > 0)
                result = value;
        }
        catch(const std::invalid_argument &) {}
        catch(const std::out_of_range &) {}
    }

    return result;
}


std::optional<float> AExifReader::readRational(const IExifReader::TagType& tagType) const
{
    std::optional<float> result;
    const std::optional<std::string> valueRaw = read(tagType);

    if (valueRaw.has_value())
    {
        try
        {
            const std::string& valueStr = *valueRaw;
            const std::size_t p = valueStr.find("/");

            if (p != std::string::npos)
            {
                const std::string nomStr = valueStr.substr(0, p);
                const std::string denStr = valueStr.substr(p + 1);
                const float nom = std::stof(nomStr);
                const float den = std::stof(denStr);

                if (std::abs(den) > 0.0001f)
                    result = nom/den;
            }
        }
        catch(const std::invalid_argument &) {}
        catch(const std::out_of_range &) {}
    }

    return result;
}

