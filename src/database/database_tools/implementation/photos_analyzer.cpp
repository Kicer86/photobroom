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

#include <core/function_wrappers.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/itask_executor.hpp>
#include <database/iphoto_operator.hpp>
#include <database/general_flags.hpp>

#include "photos_analyzer_p.hpp"
#include "../photos_analyzer.hpp"


PhotosAnalyzerImpl::PhotosAnalyzerImpl(ICoreFactoryAccessor* coreFactory, Database::IDatabase& database):
    m_updater(coreFactory, database),
    m_timer(),
    m_database(database),
    m_tasksView(nullptr),
    m_viewTask(nullptr),
    m_maxTasks(0),
    m_workers(coreFactory->getTaskExecutor().heavyWorkers()),
    m_loadingPhotos(false)
{
    connect(&m_timer, &QTimer::timeout, this, &PhotosAnalyzerImpl::refreshView);
    connect(&m_updater, &PhotoInfoUpdater::photoProcessed,
            this, &PhotosAnalyzerImpl::processPhotos);

    m_timer.start(500);

    //check for not fully initialized photos in database
    //TODO: use independent updaters here (issue #102)

    Database::FilterPhotosWithFlags flags_filter;
    flags_filter.mode = Database::FilterPhotosWithFlags::Mode::Or;

    for (auto flag : { Photo::FlagsE::ExifLoaded, Photo::FlagsE::GeometryLoaded })
        flags_filter.flags[flag] = 0;            //uninitialized

    // only normal photos
    const Database::FilterPhotosWithGeneralFlag general_flags_filter(Database::CommonGeneralFlags::State,
                                                                     static_cast<int>(Database::CommonGeneralFlags::StateType::Normal));

    const Database::GroupFilter filters = {flags_filter, general_flags_filter};

    m_database.exec([this, filters](Database::IBackend& backend)
    {
        auto photos = backend.photoOperator().getPhotos(filters);

        invokeMethod(this, &PhotosAnalyzerImpl::addPhotos, photos);

        // as all uninitialized photos were found.
        // start watching for any new photos added later.
        m_backendConnection = connect(&backend, &Database::IBackend::photosAdded,
                                      this, &PhotosAnalyzerImpl::addPhotos);
    });
}


PhotosAnalyzerImpl::~PhotosAnalyzerImpl()
{
    stop();

    if (m_viewTask)
        m_viewTask->finished();
}


void PhotosAnalyzerImpl::set(ITasksView* tasksView)
{
    m_tasksView = tasksView;
}


Database::IDatabase& PhotosAnalyzerImpl::getDatabase()
{
    return m_database;
}


void PhotosAnalyzerImpl::addPhotos(const std::vector<Photo::Id>& ids)
{
    m_photosToUpdate.insert(m_photosToUpdate.end(), ids.begin(), ids.end());

    processPhotos();
}


void PhotosAnalyzerImpl::processPhotos()
{
    if (m_loadingPhotos == false &&
        m_photosToUpdate.empty() == false &&
        m_updater.tasksInProgress() < m_workers * 2)
    {
        m_loadingPhotos = true;

        std::vector<Photo::Id> photosToProcess;
        const int toProcess = std::min(m_photosToUpdate.size(), m_workers);

        std::copy(m_photosToUpdate.begin(), m_photosToUpdate.begin() + toProcess, std::back_inserter(photosToProcess));
        m_photosToUpdate.erase(m_photosToUpdate.begin(), m_photosToUpdate.begin() + toProcess);

        m_database.exec([photosToProcess, this](Database::IBackend& backend)
        {
            std::vector<Photo::Data> photos;
            photos.reserve(m_photosToUpdate.size());

            for(const auto& id: photosToProcess)
                photos.push_back(backend.getPhoto(id));

            invokeMethod(this, &PhotosAnalyzerImpl::updatePhotos, photos);
        });
    }
}


void PhotosAnalyzerImpl::updatePhotos(const std::vector<Photo::Data>& photos)
{
    for(const auto& photo: photos)
    {
        Photo::SharedData sharedDelta(new Photo::SafeData(photo), [oldPhotoData = photo, this](Photo::SafeData* safeData)
        {
            const Photo::Data newPhotoData = *safeData->lock();
            const Photo::DataDelta delta(oldPhotoData, newPhotoData);

            m_database.exec([delta](Database::IBackend& backend)
            {
                backend.update( {delta} );
            });

            delete safeData;
        });

        if (photo.flags.at(Photo::FlagsE::GeometryLoaded) == 0)
            m_updater.updateGeometry(sharedDelta);

        if (photo.flags.at(Photo::FlagsE::ExifLoaded) == 0)
            m_updater.updateTags(sharedDelta);
    }

    m_loadingPhotos = false;
}


void PhotosAnalyzerImpl::stop()
{
    disconnect(m_backendConnection);
    m_photosToUpdate.clear();
    m_updater.waitForActiveTasks();
}


void PhotosAnalyzerImpl::setupRefresher()
{
    if (m_updater.tasksInProgress() > 0 && m_viewTask == nullptr)         //there are tasks but no view task
    {
        m_maxTasks = 0;
        m_viewTask = m_tasksView->add(tr("Loading photos data..."));
    }
    else if (m_updater.tasksInProgress() == 0 && m_photosToUpdate.size() == 0 && m_viewTask != nullptr)
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
        const int current_size = m_photosToUpdate.size() + m_updater.tasksInProgress();
        m_maxTasks = std::max(m_maxTasks, current_size);

        IProgressBar* progressBar = m_viewTask->getProgressBar();
        progressBar->setMaximum(m_maxTasks);
        progressBar->setValue(m_maxTasks - current_size);
    }
}


///////////////////////////////////////////////////////////////////////////////


PhotosAnalyzer::PhotosAnalyzer(ICoreFactoryAccessor* coreFactory,
                               Database::IDatabase& database)
    : m_data(new PhotosAnalyzerImpl(coreFactory, database))
{

}


PhotosAnalyzer::~PhotosAnalyzer()
{
    stop();
}


void PhotosAnalyzer::set(ITasksView* tasksView)
{
    m_data->set(tasksView);
}


void PhotosAnalyzer::stop()
{
    m_data->stop();
}
