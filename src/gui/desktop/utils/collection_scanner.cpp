/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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


#include <algorithm>
#include <QFileInfo>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <core/iview_task.hpp>
#include <database/iphoto_operator.hpp>
#include <database/photo_utils.hpp>
#include <database/general_flags.hpp>
#include "collection_scanner.hpp"
#include "project_utils/project.hpp"


CollectionScanner::CollectionScanner(const Project& project, ITasksView& tasksView, INotifications& notifications):
    QObject(),
    m_collector(project),
    m_state(State::Scanning),
    m_project(project),
    m_database(project.getDatabase()),
    m_tasksView(tasksView),
    m_notifications(notifications),
    m_gotPhotos(false),
    m_gotDBPhotos(false)
{
    connect(&m_collector, &PhotosCollector::finished, this, &CollectionScanner::diskScanDone);
}


CollectionScanner::~CollectionScanner()
{

}


void CollectionScanner::scan()
{
    m_progressTask = m_tasksView.add(tr("Scanning collection"));
    m_state = State::Scanning;
    updateGui();

    // collect photos from disk
    using namespace std::placeholders;
    auto disk_callback = std::bind(&CollectionScanner::gotDiskPhoto, this, _1);

    m_collector.collect(m_project.getProjectInfo().getBaseDir(), disk_callback);

    // collect photos from db
    auto db_callback = std::bind(&CollectionScanner::gotDBPhotos, this, _1);

    m_database.exec([db_callback](Database::IBackend& backend)
    {
        // collect all photos from db but those we already know are missing
        const Database::FilterPhotosWithGeneralFlag filterMissing(Database::CommonGeneralFlags::State,
                                                            static_cast<int>(Database::CommonGeneralFlags::StateType::Missing));
        const Database::FilterNotMatchingFilter filterNotMissing(filterMissing);

        auto photos = backend.photoOperator().getPhotos(filterNotMissing);

        std::vector<Photo::DataDelta> photoDeltas;
        photoDeltas.reserve(photos.size());

        for(const Photo::Id& id: photos)
            photoDeltas.push_back(backend.getPhotoDelta(id, {Photo::Field::Path}));

        db_callback(photoDeltas);
    });
}


void CollectionScanner::diskScanDone()
{
    m_gotPhotos = true;

    checkIfReady();
}


void CollectionScanner::performAnalysis()
{
    m_state = State::Analyzing;
    updateGui();

    // sort db and disk sets for further steps
    std::sort(m_dbPhotos.begin(), m_dbPhotos.end(), &Photo::isLess<Photo::Field::Path>);
    std::sort(m_diskPhotos.begin(), m_diskPhotos.end(), &Photo::isLess<Photo::Field::Path>);

    // find new photos
    std::vector<Photo::DataDelta> newPhotos;
    std::ranges::set_difference(m_diskPhotos, m_dbPhotos, std::back_inserter(newPhotos), &Photo::isLess<Photo::Field::Path>);

    // find removed photos
    std::vector<Photo::DataDelta> removedPhotos;
    std::ranges::set_difference(m_dbPhotos, m_diskPhotos, std::back_inserter(removedPhotos), &Photo::isLess<Photo::Field::Path>);

    // prepare new photos for storage
    for(auto& photo: newPhotos)
    {
        const Photo::FlagValues flags = { {Photo::FlagsE::StagingArea, 1} };

        photo.insert<Photo::Field::Flags>(flags);
    }

    // store new photos
    if (newPhotos.empty() == false)
        m_database.exec([newPhotos](Database::IBackend& backend) mutable
        {
            backend.addPhotos(newPhotos);
        });

    // mark removed photos as missing
    if (removedPhotos.empty() == false)
        m_database.exec([removedPhotos](Database::IBackend& backend) mutable
        {
            for(const auto& photo: removedPhotos)
            {
                backend.set(photo.getId(),
                            Database::CommonGeneralFlags::State,
                            static_cast<int>(Database::CommonGeneralFlags::StateType::Missing));
            }
        });

    // cleanups
    m_state = State::Done;
    updateGui();

    addNotification(newPhotos.size(), removedPhotos.size());
    m_progressTask->finished();
    m_progressTask = nullptr;
    emit scanFinished();
}


void CollectionScanner::checkIfReady()
{
    if (m_gotPhotos && m_gotDBPhotos)
        QMetaObject::invokeMethod(this, &CollectionScanner::performAnalysis, Qt::QueuedConnection);
}


void CollectionScanner::gotDiskPhoto(const QString& path)
{
    const QString relative = m_project.makePathRelative(path);
    Photo::DataDelta photo;
    photo.insert<Photo::Field::Path>(relative);
    m_diskPhotos.push_back(photo);
}


void CollectionScanner::gotDBPhotos(const std::vector<Photo::DataDelta>& photos)
{
    m_dbPhotos = photos;
    m_gotDBPhotos = true;

    checkIfReady();
}


void CollectionScanner::updateGui()
{
    switch(m_state)
    {
        case State::Canceled:
            break;

        case State::Scanning:
            break;

        case State::Analyzing:
            break;

        case State::Done:
        {
            break;
        }
    }
}


void CollectionScanner::addNotification(std::size_t added, std::size_t removed)
{
    QString info;

    if (added == 0 && removed == 0)
        info = tr("No new photos found.");
    else
    {
        if (added > 0)
            info = tr("%n new photo(s) was found and added to collection.", "", added);

        if (removed > 0)
            info += tr("%n photo(s) were no longer found on disk.", "", removed);
    }

    m_notifications.insert(info, INotifications::Type::Info);
}
