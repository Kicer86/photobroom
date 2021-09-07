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
#include <core/slicer.hpp>
#include <database/iphoto_operator.hpp>
#include <database/general_flags.hpp>
#include <database/database_executor_traits.hpp>

#include "photos_analyzer_p.hpp"
#include "../photos_analyzer.hpp"


using namespace std::chrono_literals;
using namespace std::placeholders;


PhotosAnalyzerImpl::PhotosAnalyzerImpl(ICoreFactoryAccessor* coreFactory, Database::IDatabase& database):
    m_taskQueue(&coreFactory->getTaskExecutor()),
    m_updater(m_taskQueue, coreFactory, database),
    m_timer(),
    m_updateQueue(1000, 5s, std::bind(&PhotosAnalyzerImpl::flushQueue, this, _1, _2)),
    m_database(database),
    m_tasksView(nullptr),
    m_viewTask(nullptr),
    m_maxTasks(0)
{
    connect(&m_timer, &QTimer::timeout, this, &PhotosAnalyzerImpl::refreshView);

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

        if (photos.empty() == false)
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


void PhotosAnalyzerImpl::addPhotos(const std::vector<Photo::Id>& ids)
{
    IViewTask* loadTask = m_tasksView->add(tr("Loading photos needing update"));

    runOn(m_taskQueue, [this, ids, loadTask]()
    {
        const auto count = ids.size();

        try
        {
            int progress = 0;
            loadTask->getProgressBar()->setMinimum(0);
            loadTask->getProgressBar()->setMaximum(count);

            slice(ids.begin(), ids.end(), 200, [this, loadTask, &progress](auto first, auto last)
            {
                const std::vector<Photo::Data> photoData =
                    evaluate<std::vector<Photo::Data>(Database::IBackend &)>(m_database, [first, last](Database::IBackend& backend)
                {
                    std::vector<Photo::Data> photos;
                    photos.reserve(last - first);

                    std::transform(first, last, std::back_inserter(photos), [&backend](const Photo::Id& id) mutable
                    {
                        return backend.getPhoto(id);
                    });

                    return photos;
                });

                invokeMethod(this, &PhotosAnalyzerImpl::updatePhotos, photoData);
                progress += last - first;

                loadTask->getProgressBar()->setValue(progress);

                m_workState.throwIfAbort();
            });
        }
        catch(const abort_exception &)
        {

        }

        loadTask->finished();
    },
    "Fetching photo details"
    );
}


void PhotosAnalyzerImpl::updatePhotos(const std::vector<Photo::Data>& photos)
{
    for(const auto& photo: photos)
    {
        auto storage = make_cross_thread_function<Photo::SafeData *>(this, std::bind(&PhotosAnalyzerImpl::photoUpdated, this, photo, _1));
        Photo::SharedData sharedDelta(new Photo::SafeData(photo), storage);

        if (photo.flags.at(Photo::FlagsE::GeometryLoaded) == 0)
            m_updater.updateGeometry(sharedDelta);

        if (photo.flags.at(Photo::FlagsE::ExifLoaded) == 0)
            m_updater.updateTags(sharedDelta);
    }
}


void PhotosAnalyzerImpl::photoUpdated(const Photo::Data& oldPhotoData, Photo::SafeData* safeData)
{
    const Photo::Data newPhotoData = *safeData->lock();
    const Photo::DataDelta delta(oldPhotoData, newPhotoData);

    m_updateQueue.push(delta);

    delete safeData;
}


void PhotosAnalyzerImpl::flushQueue(PhotosQueue::ContainerIt first, PhotosQueue::ContainerIt last)
{
    std::vector<Photo::DataDelta> toFlush(first, last);

    m_database.exec([toFlush](Database::IBackend& backend)
    {
        backend.update(toFlush);
    });
}


void PhotosAnalyzerImpl::stop()
{
    disconnect(m_backendConnection);
    m_taskQueue.clear();
    m_workState.abort();
    m_taskQueue.waitForPendingTasks();
}


void PhotosAnalyzerImpl::setupRefresher()
{
    if (m_taskQueue.size() > 0 && m_viewTask == nullptr)         //there are tasks but no view task
    {
        m_maxTasks = 0;
        m_viewTask = m_tasksView->add(tr("Extracting data from photos"));
    }
    else if (m_taskQueue.size() == 0 && m_viewTask != nullptr)
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
        const std::size_t current_size = m_taskQueue.size();
        m_maxTasks = std::max(m_maxTasks, current_size);

        IProgressBar* progressBar = m_viewTask->getProgressBar();
        progressBar->setMaximum(m_maxTasks);
        progressBar->setValue(m_maxTasks - current_size);
        progressBar->setFormat("%v/%m");
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

}


void PhotosAnalyzer::set(ITasksView* tasksView)
{
    m_data->set(tasksView);
}
