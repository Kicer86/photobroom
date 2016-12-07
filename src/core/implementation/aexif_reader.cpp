
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

AExifReader::AExifReader(): m_photosManager(nullptr)
{

}


AExifReader::~AExifReader()
{

}


void AExifReader::set(IPhotosManager* photosManager)
{
    m_photosManager = photosManager;
}



Tag::TagsList AExifReader::getTagsFor(const QString& path)
{
    const QByteArray data = m_photosManager->getPhoto(path);

    collect(data);

    Tag::TagsList tagData;

    feedDateAndTime(tagData);

    return tagData;
}


boost::any AExifReader::get(const QString& path, const IExifReader::ExtraData& type)
{
    const QByteArray data = m_photosManager->getPhoto(path);

    collect(data);

    boost::any result;

    switch(type)
    {
        case ExtraData::SequenceNumber:
        {
            const std::string valueRaw = read(SequenceNumber);
            const int value = stoi(valueRaw);

            if (value > 0)
                result = value;

            break;
        }
    }

    return result;
}


void AExifReader::feedDateAndTime(Tag::TagsList& tagData)
{
    const std::string dateTimeOrirignal = read(DateTimeOriginal);

    const QString v(dateTimeOrirignal.c_str());
    const QStringList time_splitted = v.split(" ");

    if (time_splitted.size() == 2)
    {
        QString date = time_splitted[0];
        const QString time = time_splitted[1];

        tagData[TagNameInfo(BaseTagsList::Date)] = TagValue(QDate::fromString(date, "yyyy:MM:dd"));
        tagData[TagNameInfo(BaseTagsList::Time)] = TagValue(QTime::fromString(time, "hh:mm:ss"));
    }
}
