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

#include <iostream>

#include "../photos_analyzer.hpp"
#include "photos_analyzer_p.hpp"


PhotosAnalyzerImpl::PhotosAnalyzerImpl(ICoreFactoryAccessor* coreFactory):
    m_updater(coreFactory),
    m_database(nullptr),
    m_timer(),
    m_tasksView(nullptr),
    m_viewTask(nullptr),
    m_maxTasks(0)
{
    connect(&m_timer, &QTimer::timeout, this, &PhotosAnalyzerImpl::refreshView);

    m_timer.start(500);
}


PhotosAnalyzerImpl::~PhotosAnalyzerImpl()
{
    stop();
}


void PhotosAnalyzerImpl::setDatabase(Database::IDatabase* database)
{
    m_database = database;

    m_updater.dropPendingTasks();
    m_updater.waitForActiveTasks();

    if (m_database != nullptr)
    {
        //check for not fully initialized photos in database

        //TODO: use independent updaters here (issue #102)

        std::shared_ptr<Database::FilterPhotosWithFlags> flags_filter = std::make_shared<Database::FilterPhotosWithFlags>();
        flags_filter->mode = Database::FilterPhotosWithFlags::Mode::Or;

        for (auto flag : { Photo::FlagsE::ExifLoaded, Photo::FlagsE::Sha256Loaded, Photo::FlagsE::GeometryLoaded })
            flags_filter->flags[flag] = 0;            //uninitialized

        const std::vector<Database::IFilter::Ptr> filters = {flags_filter};

        database->listPhotos(filters, [this](const IPhotoInfo::List& photos)
        {
            for(const IPhotoInfo::Ptr& photo: photos)
                addPhoto(photo);
        });
    }
}


void PhotosAnalyzerImpl::set(ITasksView* tasksView)
{
    m_tasksView = tasksView;
}


Database::IDatabase* PhotosAnalyzerImpl::getDatabase()
{
    return m_database;
}


void PhotosAnalyzerImpl::addPhoto(const IPhotoInfo::Ptr& photo)
{
    std::cout << "*** Adding photo with ID = " << photo->getID() << std::endl;
    assert(photo->isFullyInitialized() == false);

    if (photo->isSha256Loaded() == false)
    {
        std::cout << "*** Photo with ID = " << photo->getID() << " has no sha256" << std::endl;
        m_updater.updateSha256(photo);
    }

    if (photo->isGeometryLoaded() == false)
        m_updater.updateGeometry(photo);

    if (photo->isExifDataLoaded() == false)
        m_updater.updateTags(photo);
}


void PhotosAnalyzerImpl::stop()
{
    m_updater.dropPendingTasks();
}


void PhotosAnalyzerImpl::setupRefresher()
{
    if (m_updater.tasksInProgress() > 0 && m_viewTask == nullptr)         //there are tasks but no view task
    {
        m_maxTasks = 0;
        m_viewTask = m_tasksView->add(tr("Loading photos data..."));
    }
    else if (m_updater.tasksInProgress() == 0 && m_viewTask != nullptr)
    {
        m_viewTask->finished();
        m_viewTask = nullptr;
    }
}


void PhotosAnalyzerImpl::refreshView()
{
    setupRefresher();

    if (m_viewTask != nullptr)
    {
        const int current_size = m_updater.tasksInProgress();
        m_maxTasks = std::max(m_maxTasks, current_size);

        IProgressBar* progressBar = m_viewTask->getProgressBar();
        progressBar->setMaximum(m_maxTasks);
        progressBar->setValue(m_maxTasks - current_size);
    }
}

///////////////////////////////////////////////////////////////////////////////


PhotosAnalyzer::PhotosAnalyzer(ICoreFactoryAccessor* coreFactory): m_data(new PhotosAnalyzerImpl(coreFactory))
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

        connect(notifier, &Database::ADatabaseSignals::photosAdded, this, &PhotosAnalyzer::photosAdded, Qt::DirectConnection);
    }
}


void PhotosAnalyzer::set(ITasksView* tasksView)
{
    m_data->set(tasksView);
}


void PhotosAnalyzer::stop()
{
    m_data->stop();
}


void PhotosAnalyzer::photosAdded(const std::vector<IPhotoInfo::Ptr>& photos)
{
    for(const IPhotoInfo::Ptr& photo: photos)
        m_data->addPhoto(photo);
}
