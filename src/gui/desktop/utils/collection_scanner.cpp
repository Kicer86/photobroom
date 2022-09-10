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

    // collect photos from disk
    using namespace std::placeholders;
    auto disk_callback = std::bind(&CollectionScanner::gotDiskPhoto, this, _1);

    m_collector.collect(disk_callback);

    // collect photos from db
    auto db_callback = std::bind(&CollectionScanner::gotDBPhotos, this, _1, _2);

    m_database.exec([db_callback](Database::IBackend& backend)
    {
        // collect all (but deleted) photos however separate missing from others
        const Database::FilterNotMatchingFilter notDeleted(
            Database::FilterPhotosWithGeneralFlag(Database::CommonGeneralFlags::State,
                                                  static_cast<int>(Database::CommonGeneralFlags::StateType::Delete),
                                                  Database::FilterPhotosWithGeneralFlag::Mode::Bit)
        );

        const Database::FilterPhotosWithGeneralFlag filterMissing(Database::CommonGeneralFlags::State,
                                                                  static_cast<int>(Database::CommonGeneralFlags::StateType::Missing),
                                                                  Database::FilterPhotosWithGeneralFlag::Mode::Bit);
        const Database::FilterNotMatchingFilter filterNotMissing(filterMissing);

        auto photos = backend.photoOperator().getPhotos(Database::GroupFilter( {notDeleted, filterNotMissing} ));
        auto missingPhotos = backend.photoOperator().getPhotos(Database::GroupFilter( {notDeleted, filterMissing} ));

        std::vector<Photo::DataDelta> photoDeltas;
        photoDeltas.reserve(photos.size());

        for(const Photo::Id& id: photos)
            photoDeltas.push_back(backend.getPhotoDelta(id, {Photo::Field::Path}));

        std::vector<Photo::DataDelta> missingPhotoDeltas;
        missingPhotoDeltas.reserve(missingPhotos.size());

        for(const Photo::Id& id: missingPhotos)
            missingPhotoDeltas.push_back(backend.getPhotoDelta(id, {Photo::Field::Path}));

        db_callback(photoDeltas, missingPhotoDeltas);
    });
}


void CollectionScanner::diskScanDone()
{
    m_gotPhotos = true;

    checkIfReady();
}


void CollectionScanner::performAnalysis()
{
    // sort db and disk sets for further steps
    std::ranges::sort(m_dbPhotos, &Photo::isLess<Photo::Field::Path>);
    std::ranges::sort(m_diskPhotos, &Photo::isLess<Photo::Field::Path>);
    std::ranges::sort(m_missingPhotos, &Photo::isLess<Photo::Field::Path>);

    // find new photos
    std::vector<Photo::DataDelta> newPhotos;
    std::ranges::set_difference(m_diskPhotos, m_dbPhotos, std::back_inserter(newPhotos), &Photo::isLess<Photo::Field::Path>);

    // find removed photos
    std::vector<Photo::DataDelta> removedPhotos;
    std::ranges::set_difference(m_dbPhotos, m_diskPhotos, std::back_inserter(removedPhotos), &Photo::isLess<Photo::Field::Path>);

    // find restored photos (are marked as missing in db but now are available again)
    // it is crucial for `m_missingPhotos` to go first here as `set_intersection` will use items from this container to feed `restoredPhotos`,
    // and `m_missingPhotos` contains more details
    std::vector<Photo::DataDelta> restoredPhotos;
    std::ranges::set_intersection(m_missingPhotos, m_diskPhotos, std::back_inserter(restoredPhotos), &Photo::isLess<Photo::Field::Path>);

    // all `restored photos` will also be available in `new photos` set. Prepare a fresh container for `new photos` which are not `restored photos`
    std::vector<Photo::DataDelta> pureNewPhotos;
    std::ranges::set_difference(newPhotos, restoredPhotos, std::back_inserter(pureNewPhotos), &Photo::isLess<Photo::Field::Path>);

    // prepare new photos for storage
    for(auto& photo: pureNewPhotos)
    {
        const Photo::FlagValues flags = { {Photo::FlagsE::StagingArea, 1} };

        photo.insert<Photo::Field::Flags>(flags);
    }

    // store new photos
    if (newPhotos.empty() == false)
        m_database.exec([pureNewPhotos](Database::IBackend& backend) mutable
        {
            backend.addPhotos(pureNewPhotos);
        });

    // mark removed photos as missing
    if (removedPhotos.empty() == false)
        m_database.exec([removedPhotos](Database::IBackend& backend) mutable
        {
            for(const auto& photo: removedPhotos)
            {
                backend.setBits(photo.getId(),
                                Database::CommonGeneralFlags::State,
                                static_cast<int>(Database::CommonGeneralFlags::StateType::Missing));
            }
        });

    // restore photos
    if (restoredPhotos.empty() == false)
        m_database.exec([restoredPhotos](Database::IBackend& backend) mutable
        {
            for(const auto& photo: restoredPhotos)
            {
                backend.clearBits(photo.getId(),
                                  Database::CommonGeneralFlags::State,
                                  static_cast<int>(Database::CommonGeneralFlags::StateType::Missing));
            }
        });

    // finalization
    addNotification(pureNewPhotos.size(), removedPhotos.size(), restoredPhotos.size());
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


void CollectionScanner::gotDBPhotos(const std::vector<Photo::DataDelta>& photos, const std::vector<Photo::DataDelta>& missingPhotos)
{
    m_dbPhotos = photos;
    m_missingPhotos = missingPhotos;
    m_gotDBPhotos = true;

    checkIfReady();
}


void CollectionScanner::addNotification(std::size_t added, std::size_t removed, std::size_t restored)
{
    QString info;

    if (added == 0 && removed == 0 && restored == 0)
        info = tr("No new photos found.");
    else
    {
        if (added > 0)
            info = tr("%n new photo(s) was found and added to collection.\n", "", static_cast<int>(added));

        if (removed > 0)
            info += tr("%n photo(s) were no longer found on disk.\n", "", static_cast<int>(removed));

        if (restored > 0)
            info += tr("%n missing photo(s) were found.\n", "", static_cast<int>(restored));
    }

    m_notifications.insert(info.trimmed(), INotifications::Type::Info);
}
