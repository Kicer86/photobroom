/*
 * Synchronous database. Implementation of IDatabase
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

#include "synchronous_database.hpp"


struct SynchronousDatabase::Impl: Database::IDatabaseClient
{
    Database::IDatabase* m_database;

    Impl(): m_database(nullptr) {}

    virtual void got_getAllPhotos(const Database::Task& task, const Database::QueryList& query)
    {
    }

    virtual void got_getPhoto(const Database::Task& task, const PhotoInfo::Ptr& photo)
    {
    }

    virtual void got_getPhotos(const Database::Task& task, const Database::QueryList& query)
    {
    }

    virtual void got_listTags(const Database::Task& task, const std::vector<TagNameInfo>& tags)
    {
    }

    virtual void got_listTagValues(const Database::Task& task, const std::deque<TagValueInfo>& tags)
    {
    }

    virtual void got_storeStatus(const Database::Task& task)
    {
    }
};


SynchronousDatabase::SynchronousDatabase(): m_impl(new Impl)
{

}


SynchronousDatabase::~SynchronousDatabase()
{

}


void SynchronousDatabase::setDatabase(Database::IDatabase* database)
{
    m_impl->m_database = database;
}


void SynchronousDatabase::closeConnections()
{
    m_impl->m_database->closeConnections();
}


void SynchronousDatabase::getAllPhotos(const Database::Task& task)
{
    m_impl->m_database->getAllPhotos(task);
}


void SynchronousDatabase::getPhoto(const Database::Task& task, const PhotoInfo::Id& id)
{
    m_impl->m_database->getPhoto(task, id);
}


void SynchronousDatabase::getPhotos(const Database::Task& task, const std::deque<Database::IFilter::Ptr>& filters)
{
    m_impl->m_database->getPhotos(task, filters);
}


bool SynchronousDatabase::init(const Database::Task& task, const std::string& s)
{
    return m_impl->m_database->init(task, s);
}


void SynchronousDatabase::listTags(const Database::Task& task)
{
    m_impl->m_database->listTags(task);
}


void SynchronousDatabase::listTagValues(const Database::Task& task, const TagNameInfo& tag)
{
    m_impl->m_database->listTagValues(task, tag);
}


void SynchronousDatabase::listTagValues(const Database::Task& task, const TagNameInfo& tag, const std::deque<Database::IFilter::Ptr>& filters)
{
    m_impl->m_database->listTagValues(task, tag, filters);
}


Database::ADatabaseSignals* SynchronousDatabase::notifier()
{
    return m_impl->m_database->notifier();
}


Database::Task SynchronousDatabase::prepareTask(Database::IDatabaseClient* client)
{
    return m_impl->m_database->prepareTask(client);
}


void SynchronousDatabase::store(const Database::Task& task, const PhotoInfo::Ptr& photo)
{
    m_impl->m_database->store(task, photo);
}
