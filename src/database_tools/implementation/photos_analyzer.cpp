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

#include "photos_analyzer.hpp"

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
        PhotosAnalyzerThread(): m_data_available(), m_data_mutex(), m_photosToUpdate(), m_work(true), m_updater()
        {
        }

        void execute()
        {
            std::unique_lock<std::mutex> lock(m_data_mutex);

            while (m_work)
            {
                m_data_available.wait(lock, [&]{ return m_photosToUpdate.lock()->empty() == false || m_work == false; });

                IPhotoInfo::Ptr photoInfo(nullptr);
                {
                    auto photosToUpdate = m_photosToUpdate.lock();

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

        std::condition_variable m_data_available;
        std::mutex m_data_mutex;
        ThreadSafeResource<std::deque<IPhotoInfo::Ptr>> m_photosToUpdate;
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

    Database::ADatabaseSignals* getNotifier()
    {
        return m_database->notifier();
    }

    void addPhoto(const IPhotoInfo::Ptr& photo)
    {
        m_thread.m_photosToUpdate.lock()->push_back(photo);
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


void PhotosAnalyzer::setDatabase(Database::IDatabase* database)
{
    m_data->setDatabase(database);
    auto notifier = m_data->getNotifier();

    connect(notifier, SIGNAL(photoAdded(IPhotoInfo::Ptr)), this, SLOT(photoAdded(IPhotoInfo::Ptr)), Qt::DirectConnection);
}


void PhotosAnalyzer::photoAdded(const IPhotoInfo::Ptr& photo)
{
    m_data->addPhoto(photo);
}
