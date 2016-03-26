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
        typedef std::function<void(const QString &, const IPhotoInfo::List &)> Callback;

        PathCheckTask(const QString& path, const Callback& callback): m_callback(callback), m_path(path) {}

        void got(const IPhotoInfo::List& photos) override
        {
            m_callback(m_path, photos);
        }

        Callback m_callback;
        QString  m_path;
    };
}


PathChecker::PathChecker(): m_database(nullptr), m_callbackCtrl()
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
    // prepare filters
    std::deque<Database::IFilter::Ptr> filters;
    auto filter = std::make_shared<Database::FilterPhotosWithPath>(path);
    filters.push_back(filter);

    // prepare callback
    using namespace std::placeholders;
    auto callback = std::bind(&PathChecker::gotPhotos, this, _1, _2);
    auto safeCallback = m_callbackCtrl.make_safe_callback<void(const QString &, const IPhotoInfo::List&)>(callback);

    // execute task
    auto pathCheckTask = std::make_unique<PathCheckTask>(path, safeCallback);
    m_database->exec( std::move(pathCheckTask), filters );
}


void PathChecker::gotPhotos(const QString& path, const IPhotoInfo::List& photos)
{
    Photo::Id id;

    if (photos.empty() == false)
    {
        assert(photos.size() == 1);
        id = photos.front()->getID();
    }

    emit fileChecked(path, id);
}
