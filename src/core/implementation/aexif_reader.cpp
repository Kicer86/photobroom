
/*
* Base for tag feeders
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
#include <QStringList>
#include <QDate>
#include <QTime>

#include "photos_manager.hpp"
#include "tag.hpp"
#include "base_tags.hpp"



AExifReader::AExifReader(IPhotosManager* photosManager): m_id(std::this_thread::get_id()), m_photosManager(photosManager)
{

}


AExifReader::~AExifReader()
{

}


Tag::TagsList AExifReader::getTagsFor(const QString& path)
{
    assert(m_id == std::this_thread::get_id());

    const QByteArray data = m_photosManager->getPhoto(path);
    collect(data);

    const Tag::TagsList tagData = feedDateAndTime();

    return tagData;
}


boost::any AExifReader::get(const QString& path, const IExifReader::TagType& type)
{
    assert(m_id == std::this_thread::get_id());

    const QByteArray data = m_photosManager->getPhoto(path);

    collect(data);

    boost::any result;

    switch(type)
    {
        case TagType::SequenceNumber:
            result = readInt(TagType::SequenceNumber);
            break;

        case TagType::Orientation:
            result = readInt(TagType::Orientation);
            break;

        case TagType::DateTimeOriginal:
            result = readString(TagType::DateTimeOriginal);
            break;
    }

    return result;
}


Tag::TagsList AExifReader::feedDateAndTime() const
{
    Tag::TagsList tagData;

    const std::string dateTimeOrirignal = read(TagType::DateTimeOriginal);

    const QString v(dateTimeOrirignal.c_str());
    const QStringList time_splitted = v.split(" ");

    if (time_splitted.size() == 2)
    {
        QString date_raw = time_splitted[0];
        const QString time_raw = time_splitted[1];

        const QDate date = QDate::fromString(date_raw, "yyyy:MM:dd");
        const QTime time = QTime::fromString(time_raw, "hh:mm:ss");

        if (date.isValid())
            tagData[TagNameInfo(BaseTagsList::Date)] = TagValue(date);

        if (time.isValid())
            tagData[TagNameInfo(BaseTagsList::Time)] = TagValue(time);
    }

    return tagData;
}


int AExifReader::readInt(const TagType& tagType) const
{
    int result = 0;
    const std::string valueRaw = read(tagType);

    try
    {
        const int value = stoi(valueRaw);

        if (value > 0)
            result = value;
    }
    catch(const std::invalid_argument &) {}
    catch(const std::out_of_range &) {}

    return result;
}


std::string AExifReader::readString(const TagType& tagType) const
{
    const std::string result = read(tagType);

    return result;
}
