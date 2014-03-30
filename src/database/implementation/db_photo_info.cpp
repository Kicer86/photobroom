/*
 * Implementation of APhotoInfo for DataBase
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

#include "db_photo_info.hpp"

#include <core/tag.hpp>

#include "photo_iterator.hpp"


DBPhotoInfo::DBPhotoInfo(const Database::InterfaceContainer<Database::IQuery>& q, const APhotoInfoInitData& data): APhotoInfo(data)
{
    const Database::InterfaceContainer<Database::IQuery> query = q;

    //current photo's id
    const unsigned int id = query->getField(Database::IQuery::Fields::Id).toInt();
    bool n = true;
    std::shared_ptr<ITagData> tags = APhotoInfo::getTags();

    //gather tags until we get into next photo
    while (n)
    {
        const unsigned int n_id = query->getField(Database::IQuery::Fields::Id).toInt();

        if (n_id == id) // still the same photo?
        {
            const unsigned int type = query->getField(Database::IQuery::Fields::TagType).toInt();
            const QString name  = query->getField(Database::IQuery::Fields::TagName).toString();
            const QString value = query->getField(Database::IQuery::Fields::TagValue).toString();

            const TagNameInfo tagInfo(name, type);
            tags->setTag(tagInfo, value);

            //goto next row
            query->gotoNext();
        }
        else
            n = false;   //stop working. query points to next photo
    }
}


DBPhotoInfo::~DBPhotoInfo()
{

}


const RawPhotoData& DBPhotoInfo::rawThumbnailData()
{

}


const RawPhotoData& DBPhotoInfo::rawPhotoData()
{

}
