/*
 * Photo analyzer and updater.
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

#include "../photos_analyzer.hpp"

#include <thread>
#include <condition_variable>
#include <mutex>

#include <OpenLibrary/putils/ts_resource.hpp>

#include <database/idatabase.hpp>

#include "photo_info_updater.hpp"


namespace
{
    struct PhotosAnalyzerThread
    {
        PhotosAnalyzerThread(): m_data_available(), m_data_mutex(), m_photosToValidate(), m_work(true), m_updater()
        {
        }

        void execute()
        {
            std::unique_lock<std::mutex> lock(m_data_mutex);

            while (m_work)
            {
                m_data_available.wait(lock, [&] { return m_photosToValidate.lock()->empty() == false || m_work == false; });

                IPhotoInfo::Ptr photoInfo(nullptr);
                {
                    auto photosToUpdate = m_photosToValidate.lock();

                    if (photosToUpdate->empty() == false)
                    {
                        photoInfo = photosToUpdate->front();
                        photosToUpdate->pop_front();
                    }
                }

                if (photoInfo.get() != nullptr)
                    process(photoInfo);
            }

        }


        //TODO: use list of updaters (introduce updater interface)
        void process(const IPhotoInfo::Ptr& photoInfo)
        {
            if (photoInfo->isFullyInitialized() == false)
            {
                if (photoInfo->isHashLoaded() == false)
                    m_updater.updateHash(photoInfo);

                if (photoInfo->isThumbnailLoaded() == false)
                    m_updater.updateThumbnail(photoInfo);

                if (photoInfo->isExifDataLoaded() == false)
                    m_updater.updateTags(photoInfo);
            }
        }

        void set(ITaskExecutor* taskExecutor)
        {
            m_updater.set(taskExecutor);
        }

        void set(IConfiguration* configuration)
        {
            m_updater.set(configuration);
        }

        std::condition_variable m_data_available;
        std::mutex m_data_mutex;
        ol::ThreadSafeResource<std::deque<IPhotoInfo::Ptr>> m_photosToValidate;
        bool m_work;
        PhotoInfoUpdater m_updater;
    };

    void trampoline(PhotosAnalyzerThread* thread)
    {
        thread->execute();
    }
}


///////////////////////////////////////////////////////////////////////////////


struct PhotosAnalyzer::Impl
{
        Impl(): m_database(nullptr), m_thread(), m_analyzerThread(trampoline, &m_thread)
        {

        }

        Impl(const Impl &) = delete;
        Impl& operator=(const Impl &) = delete;

        ~Impl()
        {
            m_thread.m_work = false;
            m_thread.m_data_available.notify_one();

            assert(m_analyzerThread.joinable());
            m_analyzerThread.join();
        }

        void setDatabase(Database::IDatabase* database)
        {
            m_database = database;
        }

        void set(ITaskExecutor* taskExecutor)
        {
            m_thread.set(taskExecutor);
        }

        void set(IConfiguration* configuration)
        {
            m_thread.set(configuration);
        }

        Database::IDatabase* getDatabase()
        {
            return m_database;
        }

        void addPhoto(const IPhotoInfo::Ptr& photo)
        {
            m_thread.m_photosToValidate.lock()->push_back(photo);
            m_thread.m_data_available.notify_one();
        }

    private:
        Database::IDatabase* m_database;
        PhotosAnalyzerThread m_thread;
        std::thread m_analyzerThread;
};

///////////////////////////////////////////////////////////////////////////////


PhotosAnalyzer::PhotosAnalyzer(): m_data(new Impl)
{

}


PhotosAnalyzer::~PhotosAnalyzer()
{
    delete m_data, m_data = nullptr;
}


void PhotosAnalyzer::setDatabase(Database::IDatabase* new_database)
{
    //disconnect current database
    Database::IDatabase* cur_database = m_data->getDatabase();

    if (cur_database)
    {
        Database::ADatabaseSignals* notifier = cur_database->notifier();

        notifier->disconnect(this);
    }

    //setup new database
    m_data->setDatabase(new_database);

    //and new connections
    if (new_database)
    {
        Database::ADatabaseSignals* notifier = new_database->notifier();

        connect(notifier, SIGNAL(photoAdded(IPhotoInfo::Ptr)), this, SLOT(photoAdded(IPhotoInfo::Ptr)), Qt::DirectConnection);
    }
}


void PhotosAnalyzer::set(ITaskExecutor* taskExecutor)
{
    m_data->set(taskExecutor);
}


void PhotosAnalyzer::set(IConfiguration* configuration)
{
    m_data->set(configuration);
}


void PhotosAnalyzer::photoAdded(const IPhotoInfo::Ptr& photo)
{
    m_data->addPhoto(photo);
}

