/*
 * Photo Broom - photos management tool.
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

#include <database/iphoto_operator.hpp>
#include "photos_analyzer_p.hpp"
#include "../photos_analyzer.hpp"


PhotosAnalyzerImpl::PhotosAnalyzerImpl(ICoreFactoryAccessor* coreFactory):
    m_updater(coreFactory),
    m_timer(),
    m_database(nullptr),
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
    if (m_database != nullptr)
    {
        bool status = disconnect(&m_signalMapper, &Database::SignalMapper::photosAdded,
                                 this, &PhotosAnalyzerImpl::newPhotosAdded);

        assert(status);
    }

    m_database = database;
    m_signalMapper.set(database);

    m_updater.dropPendingTasks();
    m_updater.waitForActiveTasks();

    if (m_database != nullptr)
    {
        //check for not fully initialized photos in database

        //TODO: use independent updaters here (issue #102)

        Database::FilterPhotosWithFlags flags_filter;
        flags_filter.mode = Database::FilterPhotosWithFlags::Mode::Or;

        for (auto flag : { Photo::FlagsE::ExifLoaded, Photo::FlagsE::Sha256Loaded, Photo::FlagsE::GeometryLoaded })
            flags_filter.flags[flag] = 0;            //uninitialized

        m_database->exec([this, flags_filter](Database::IBackend& backend)
        {
            auto photos = backend.photoOperator().getPhotos(flags_filter);

            for(const Photo::Id& id: photos)
                addPhoto(m_database->utils().getPhotoFor(id));

            // as all uninitialized photos were processed.
            // start watching for any new photos added later.
            connect(&m_signalMapper, &Database::SignalMapper::photosAdded,
                    this, &PhotosAnalyzerImpl::newPhotosAdded,
                    Qt::DirectConnection);
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


Database::SignalMapper* PhotosAnalyzerImpl::getMapper()
{
    return &m_signalMapper;
}


void PhotosAnalyzerImpl::addPhoto(const IPhotoInfo::Ptr& photo)
{
    assert(photo->isFullyInitialized() == false);

    if (photo->isSha256Loaded() == false)
        m_updater.updateSha256(photo);

    if (photo->isGeometryLoaded() == false)
        m_updater.updateGeometry(photo);

    if (photo->isExifDataLoaded() == false)
        m_updater.updateTags(photo);
}


void PhotosAnalyzerImpl::newPhotosAdded(const std::vector<IPhotoInfo::Ptr>& photos)
{
    for(const IPhotoInfo::Ptr& photo: photos)
        addPhoto(photo);
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

}


void PhotosAnalyzer::setDatabase(Database::IDatabase* new_database)
{
    m_data->setDatabase(new_database);
}


void PhotosAnalyzer::set(ITasksView* tasksView)
{
    m_data->set(tasksView);
}


void PhotosAnalyzer::stop()
{
    m_data->stop();
}
