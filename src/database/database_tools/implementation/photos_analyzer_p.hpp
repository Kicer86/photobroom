/*
 * Private part of PhotosAnalyzer
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

#ifndef PHOTOSANALYZER_PRIVATE_HPP
#define PHOTOSANALYZER_PRIVATE_HPP


#include <thread>
#include <condition_variable>
#include <mutex>

#include <QTimer>

#include <core/itasks_view.hpp>
#include <core/iview_task.hpp>
#include <database/idatabase.hpp>

#include "photo_info_updater.hpp"


class PhotosAnalyzerImpl;


struct IncompletePhotos: Database::AGetPhotosTask
{
    IncompletePhotos(PhotosAnalyzerImpl* impl): m_analyzerImpl(impl) {}
    IncompletePhotos(const IncompletePhotos &) = delete;

    IncompletePhotos& operator=(const IncompletePhotos &) = delete;

    virtual ~IncompletePhotos() {}

    void got(const IPhotoInfo::List& photos) override;

    PhotosAnalyzerImpl* m_analyzerImpl;
};


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

        dropPendingTasks();
    }


    //TODO: use list of updaters (introduce updater interface)
    void process(const IPhotoInfo::Ptr& photoInfo)
    {
        if (photoInfo->isFullyInitialized() == false)
        {
            if (photoInfo->isSha256Loaded() == false)
                m_updater.updateSha256(photoInfo);

            if (photoInfo->isThumbnailLoaded() == false)
                m_updater.updateThumbnail(photoInfo);

            if (photoInfo->isExifDataLoaded() == false)
                m_updater.updateTags(photoInfo);
        }
    }

    void dropPendingTasks()
    {
        // drop any not processed photos
        m_photosToValidate.lock()->clear();

        // wait for tasks being processed
        m_updater.waitForPendingTasks();

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



class PhotosAnalyzerImpl: public QObject
{
    public:
        PhotosAnalyzerImpl():
            m_database(nullptr),
            m_thread(),
            m_analyzerThread(),
            m_timer(),
            m_tasksView(nullptr),
            m_viewTask(nullptr),
            m_maxTasks(0)
        {
            connect(&m_timer, &QTimer::timeout, this, &PhotosAnalyzerImpl::refreshView);

            m_timer.start(500);

            m_analyzerThread = std::thread([&]
            {
                m_thread.execute();
            });
        }

        PhotosAnalyzerImpl(const PhotosAnalyzerImpl&) = delete;
        PhotosAnalyzerImpl& operator=(const PhotosAnalyzerImpl&) = delete;

        ~PhotosAnalyzerImpl()
        {
            stop();
        }

        void setDatabase(Database::IDatabase* database)
        {
            m_database = database;

            m_thread.dropPendingTasks();

            if (m_database != nullptr)
            {
                //check for not fully initialized photos in database

                //TODO: use independent updaters here (issue #102)

                std::shared_ptr<Database::FilterPhotosWithFlags> flags_filter = std::make_shared<Database::FilterPhotosWithFlags>();
                flags_filter->mode = Database::FilterPhotosWithFlags::Mode::Or;

                for (auto flag :
                        {
                            IPhotoInfo::FlagsE::ExifLoaded, IPhotoInfo::FlagsE::Sha256Loaded, IPhotoInfo::FlagsE::ThumbnailLoaded
                        })
                    flags_filter->flags[flag] = 0;            //uninitialized

                IncompletePhotos* task = new IncompletePhotos(this);
                const std::deque<Database::IFilter::Ptr> filters = {flags_filter};

                database->exec(std::unique_ptr<IncompletePhotos>(task), filters);
            }
        }

        void set(ITaskExecutor* taskExecutor)
        {
            m_thread.set(taskExecutor);
        }

        void set(IConfiguration* configuration)
        {
            m_thread.set(configuration);
        }

        void set(ITasksView* tasksView)
        {
            m_tasksView = tasksView;
        }

        Database::IDatabase* getDatabase()
        {
            return m_database;
        }

        void addPhoto(const IPhotoInfo::Ptr& photo)
        {
            assert(m_analyzerThread.joinable());
            m_thread.m_photosToValidate.lock()->push_back(photo);
            m_thread.m_data_available.notify_one();
        }

        void stop()
        {
            if (m_thread.m_work)
            {
                m_thread.m_work = false;
                m_thread.m_data_available.notify_one();

                assert(m_analyzerThread.joinable());
                m_analyzerThread.join();
            }
        }

    private:
        Database::IDatabase* m_database;
        PhotosAnalyzerThread m_thread;
        std::thread m_analyzerThread;
        QTimer m_timer;
        ITasksView* m_tasksView;
        IViewTask* m_viewTask;
        int m_maxTasks;

        void setupRefresher(const ol::ThreadSafeResource <std::deque<IPhotoInfo::Ptr>>::Accessor& photos)
        {
            if (photos->empty() == false && m_viewTask == nullptr)         //there are tasks but no view task
            {
                m_maxTasks = 0;
                m_viewTask = m_tasksView->add(tr("Loading photos data..."));
            }
            else
                if (photos->empty() && m_viewTask != nullptr)
                {
                    m_viewTask->finished();
                    m_viewTask = nullptr;
                }
        }

        void refreshView()
        {
            auto photos = m_thread.m_photosToValidate.lock();
            setupRefresher(photos);

            if (m_viewTask != nullptr)
            {
                const int current_size = photos->size();
                m_maxTasks = std::max(m_maxTasks, current_size);

                IProgressBar* progressBar = m_viewTask->getProgressBar();
                progressBar->setMaximum(m_maxTasks);
                progressBar->setValue(m_maxTasks - current_size);
            }
        }
};

#endif // PHOTOSANALYZER_PRIVATE_HPP

