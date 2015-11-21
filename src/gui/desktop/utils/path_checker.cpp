/*
 * Utility for checking if path already exists in database
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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


#include "path_checker.hpp"

#include <functional>

#include <database/idatabase.hpp>


namespace
{
    struct PathCheckTask: Database::AGetPhotosTask
    {
        typedef callback_ptr2<std::function<void(const IPhotoInfo::List &)>> Callback;

        PathCheckTask(const Callback& callback): m_callback(callback) {}

        void got(const IPhotoInfo::List& photos) override
        {
            m_callback(photos);
        }

        Callback m_callback;
    };
}


PathChecker::PathChecker(): m_cache(), m_database(nullptr), m_callbackCtrl()
{

}


PathChecker::~PathChecker()
{

}


void PathChecker::set(Database::IDatabase* db)
{
    m_database = db;
}


void PathChecker::checkFile(const QString& path)
{
    auto it = m_cache.find(path);

    if (it == m_cache.end())
    {
        // prepare filters
        std::deque<Database::IFilter::Ptr> filters;
        auto filter = std::make_shared<Database::FilterPhotosWithPath>(path);
        filters.push_back(filter);

        // prepare callback
        using namespace std::placeholders;
        std::function<void(const IPhotoInfo::List &)> callback = std::bind(&PathChecker::gotPhotos, this, _1);
        auto callbackPtr = m_callbackCtrl.get_callback(callback);

        // execute task
        auto pathCheckTask = std::make_unique<PathCheckTask>(callbackPtr);
        m_database->exec( std::move(pathCheckTask), filters );
    }
    else
        emit fileChecked(it->first, it->second);
}


void PathChecker::gotPhotos(const IPhotoInfo::List& photos)
{

}
