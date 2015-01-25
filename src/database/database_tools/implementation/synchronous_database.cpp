/*
 * Decorator for IDatabase which makes it synchronous
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

#include <condition_variable>
#include <mutex>

#include <database/idatabase.hpp>

namespace
{
    struct DataReceiver
    {
        DataReceiver(): m_photo_list(), m_cv(), m_data_mutex(), m_got_data(false)
        {}

        void got_getPhotos(const IPhotoInfo::List& photos)
        {
            m_photo_list = photos;

            m_got_data = true;
            m_cv.notify_one();
        }

        IPhotoInfo::List m_photo_list;

        std::condition_variable m_cv;
        std::mutex m_data_mutex;
        bool m_got_data;
    };


    struct GetPhotos: Database::AGetPhotosTask
    {
        GetPhotos(DataReceiver* dr): m_dataReceiver(dr) {}
        GetPhotos(const GetPhotos &) = delete;
        virtual ~GetPhotos(){ }

        GetPhotos& operator=(const GetPhotos &) = delete;

        virtual void got(const IPhotoInfo::List& photos) override
        {
            m_dataReceiver->got_getPhotos(photos);
        }

        DataReceiver* m_dataReceiver;
    };
}


SynchronousDatabase::SynchronousDatabase(): m_database(nullptr)
{

}


SynchronousDatabase::~SynchronousDatabase()
{

}


void SynchronousDatabase::set(Database::IDatabase* database)
{
    m_database = database;
}


const IPhotoInfo::List SynchronousDatabase::getPhotos(const std::deque< Database::IFilter::Ptr >& filters)
{
    DataReceiver receiver;

    std::unique_ptr<Database::AGetPhotosTask> task(new GetPhotos(&receiver));
    m_database->exec(std::move(task), filters);

    std::unique_lock<std::mutex> lock(receiver.m_data_mutex);

    receiver.m_cv.wait(lock, [&]()
    {
        return receiver.m_got_data == true;
    });

    return receiver.m_photo_list;
}
