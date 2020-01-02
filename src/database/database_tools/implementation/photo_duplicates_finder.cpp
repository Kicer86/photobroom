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

#include "photo_duplicates_finder.hpp"

#include <database/idatabase.hpp>

#include "synchronous_database.hpp"


PhotoDuplicatesFinder::PhotoDuplicatesFinder(): m_database(nullptr)
{

}


PhotoDuplicatesFinder::~PhotoDuplicatesFinder()
{

}


void PhotoDuplicatesFinder::setDatabase(Database::IDatabase* database)
{
    m_database = database;
}


bool PhotoDuplicatesFinder::hasDuplicate(const IPhotoInfo::Ptr& photo) const
{
    //const QString& path = photo->getPath();

    SynchronousDatabase database;

    auto shaFilter = std::make_shared<Database::FilterPhotosWithSha256>();
    shaFilter->sha256 = photo->getSha256();
    
    std::vector<Database::IFilter::Ptr> filters( {shaFilter} );

    database.set(m_database);
    auto photos = database.getPhotos(filters);

    bool result = false;

    /*
    if (photos.size() == 1)
    {
        result = true;
        assert(photos[0]->getID() == photo->getID());    // the photo we received should be the one we got in `photo`
    }
    else if (photos.empty())
        assert(!"Should not happend");

    */

    return result;
}
