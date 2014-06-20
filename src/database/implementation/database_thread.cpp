/*
 * Database thread.
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

#include "database_thread.hpp"


namespace Database
{

    void DatabaseThread::closeConnections()
    {

    }

    bool DatabaseThread::init(const char* )
    {

    }

    PhotoInfo::Ptr DatabaseThread::getPhoto(const PhotoInfo::Id& )
    {

    }

    Database::QueryList DatabaseThread::getPhotos(const std::deque<Database::IFilter::Ptr>& )
    {

    }

    Database::QueryList DatabaseThread::getAllPhotos()
    {

    }

    std::deque<TagValueInfo> DatabaseThread::listTagValues(const TagNameInfo& , const std::deque<Database::IFilter::Ptr>& )
    {

    }

    std::set<TagValueInfo> DatabaseThread::listTagValues(const TagNameInfo& )
    {

    }

    std::vector<TagNameInfo> DatabaseThread::listTags()
    {

    }

    bool DatabaseThread::store(const PhotoInfo::Ptr& )
    {

    }

}
