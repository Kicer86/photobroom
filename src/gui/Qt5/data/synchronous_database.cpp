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
    IWaiter* m_waiter;
    Database::IDatabaseClient* m_client;

    Impl(): m_database(nullptr), m_waiter(nullptr), m_client(nullptr) {}

    void wait()
    {
        m_waiter->wait();
    }

    Database::Task generateTask(const Database::Task& task)
    {
        Database::Task result(this, task.getId());

        return result;
    }

    //Database::IDatabaseClient:
    void got_getAllPhotos(const Database::Task& task, const Database::QueryList& query) override
    {
        m_client->got_getAllPhotos(task, query);
        m_waiter->stop();
    }

    void got_getPhoto(const Database::Task& task, const PhotoInfo::Ptr& photo) override
    {
        m_client->got_getPhoto(task, photo);
        m_waiter->stop();
    }

    void got_getPhotos(const Database::Task& task, const Database::QueryList& query) override
    {
        m_client->got_getPhotos(task, query);
        m_waiter->stop();
    }

    void got_listTags(const Database::Task& task, const std::vector<TagNameInfo>& tags) override
    {
        m_client->got_listTags(task, tags);
        m_waiter->stop();
    }

    void got_listTagValues(const Database::Task& task, const std::deque<TagValueInfo>& tags) override
    {
        m_client->got_listTagValues(task, tags);
        m_waiter->stop();
    }

    void got_storeStatus(const Database::Task& task) override
    {
        m_client->got_storeStatus(task);
        m_waiter->stop();
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


void SynchronousDatabase::setWaiter(SynchronousDatabase::IWaiter* waiter)
{
    m_impl->m_waiter = waiter;
}


void SynchronousDatabase::closeConnections()
{
    m_impl->m_database->closeConnections();
}


void SynchronousDatabase::getAllPhotos(const Database::Task& task)
{
    Database::Task newTask = m_impl->generateTask(task);
    m_impl->m_database->getAllPhotos(newTask);
    m_impl->wait();
}


void SynchronousDatabase::getPhoto(const Database::Task& task, const PhotoInfo::Id& id)
{
    Database::Task newTask = m_impl->generateTask(task);
    m_impl->m_database->getPhoto(newTask, id);
    m_impl->wait();
}


void SynchronousDatabase::getPhotos(const Database::Task& task, const std::deque<Database::IFilter::Ptr>& filters)
{
    Database::Task newTask = m_impl->generateTask(task);
    m_impl->m_database->getPhotos(newTask, filters);
    m_impl->wait();
}


bool SynchronousDatabase::init(const Database::Task& task, const std::string& s)
{
    return m_impl->m_database->init(task, s);
}


void SynchronousDatabase::listTags(const Database::Task& task)
{
    Database::Task newTask = m_impl->generateTask(task);
    m_impl->m_database->listTags(newTask);
    m_impl->wait();
}


void SynchronousDatabase::listTagValues(const Database::Task& task, const TagNameInfo& tag)
{
    Database::Task newTask = m_impl->generateTask(task);
    m_impl->m_database->listTagValues(newTask, tag);
    m_impl->wait();
}


void SynchronousDatabase::listTagValues(const Database::Task& task, const TagNameInfo& tag, const std::deque<Database::IFilter::Ptr>& filters)
{
    Database::Task newTask = m_impl->generateTask(task);
    m_impl->m_database->listTagValues(newTask, tag, filters);
    m_impl->wait();
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
    Database::Task newTask = m_impl->generateTask(task);
    m_impl->m_database->store(newTask, photo);
    m_impl->wait();
}
