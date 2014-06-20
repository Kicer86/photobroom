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

    DatabaseThread::DatabaseThread(IBackend* backend): m_backend(backend)
    {

    }


    DatabaseThread::~DatabaseThread()
    {

    }


    void DatabaseThread::closeConnections()
    {
        m_backend->closeConnections();
    }


    bool DatabaseThread::init(const char* name)
    {
        const bool status = m_backend->init(name);
        return status;
    }


    bool DatabaseThread::store(const PhotoInfo::Ptr& photo)
    {
        const bool status = m_backend->store(photo);
        return status;
    }


    void DatabaseThread::getAllPhotos(IDatabaseClient* client)
    {
        auto result = m_backend->getAllPhotos();
        client->got_getAllPhotos(result);
    }


    void DatabaseThread::getPhoto(const PhotoInfo::Id& id, IDatabaseClient* client)
    {
        auto result = m_backend->getPhoto(id);
        client->got_getPhoto(result);
    }


    void DatabaseThread::getPhotos(const std::deque<IFilter::Ptr>& filter, IDatabaseClient* client)
    {
        auto result = m_backend->getPhotos(filter);
        client->got_getPhotos(result);
    }


    void DatabaseThread::listTags(IDatabaseClient* client)
    {
        auto result = m_backend->listTags();
        client->got_listTags(result);
    }


    void DatabaseThread::listTagValues(const TagNameInfo& info, IDatabaseClient* client)
    {
        auto result = m_backend->listTagValues(info);
        client->got_listTagValues(result);
    }


    void DatabaseThread::listTagValues(const TagNameInfo& info, const std::deque<IFilter::Ptr>& filter, IDatabaseClient* client)
    {
        auto result = m_backend->listTagValues(info, filter);
        client->got_listTagValues(result);
    }

}
