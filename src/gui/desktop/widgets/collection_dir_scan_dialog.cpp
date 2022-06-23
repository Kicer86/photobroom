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


#include <QFileInfo>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <core/iview_task.hpp>
#include <database/iphoto_operator.hpp>
#include "collection_dir_scan_dialog.hpp"
#include "project_utils/project.hpp"


CollectionDirScanDialog::CollectionDirScanDialog(const Project* project, Database::IDatabase& db, ITasksView& tasksView, INotifications& notifications):
    QObject(),
    m_collector(project),
    m_photosFound(),
    m_dbPhotos(),
    m_state(State::Scanning),
    m_project(project),
    m_database(db),
    m_tasksView(tasksView),
    m_notifications(notifications),
    m_gotPhotos(false),
    m_gotDBPhotos(false)
{
    connect(&m_collector, &PhotosCollector::finished, this, &CollectionDirScanDialog::scanDone);
}


CollectionDirScanDialog::~CollectionDirScanDialog()
{

}


void CollectionDirScanDialog::scan()
{
    m_progressTask = m_tasksView.add(tr("Scanning collection"));
    m_state = State::Scanning;
    updateGui();

    // collect photos from disk
    using namespace std::placeholders;
    auto disk_callback = std::bind(&CollectionDirScanDialog::gotPhoto, this, _1);

    m_collector.collect(m_project->getProjectInfo().getBaseDir(), disk_callback);

    // collect photos from db
    auto db_callback = std::bind(&CollectionDirScanDialog::gotExistingPhotos, this, _1);

    m_database.exec([db_callback](Database::IBackend& backend)
    {
        auto photos = backend.photoOperator().getPhotos(Database::EmptyFilter());

        std::vector<Photo::DataDelta> photoDeltas;
        photoDeltas.reserve(photos.size());

        for(const Photo::Id& id: photos)
            photoDeltas.push_back(backend.getPhotoDelta(id, {Photo::Field::Path}));

        db_callback(photoDeltas);
    });
}


void CollectionDirScanDialog::scanDone()
{
    m_gotPhotos = true;

    checkIfReady();
}


void CollectionDirScanDialog::performAnalysis()
{
    m_state = State::Analyzing;
    updateGui();

    for(const Photo::DataDelta& photo: m_dbPhotos)
    {
        const QString path = photo.get<Photo::Field::Path>();

        auto it = m_photosFound.find(path);

        if (it != m_photosFound.end())
            m_photosFound.erase(it);
    }

    // now m_photosFound contains only photos which are not in db
    m_state = State::Done;
    updateGui();

    m_progressTask->finished();
    m_progressTask = nullptr;
}


void CollectionDirScanDialog::checkIfReady()
{
    if (m_gotPhotos && m_gotDBPhotos)
        QMetaObject::invokeMethod(this, &CollectionDirScanDialog::performAnalysis, Qt::QueuedConnection);
}


void CollectionDirScanDialog::gotPhoto(const QString& path)
{
    const QString relative = m_project->makePathRelative(path);
    m_photosFound.insert(relative);
}


void CollectionDirScanDialog::gotExistingPhotos(const std::vector<Photo::DataDelta>& photos)
{
    m_dbPhotos = photos;
    m_gotDBPhotos = true;

    checkIfReady();
}


void CollectionDirScanDialog::updateGui()
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
            const QString info = m_photosFound.empty()?
                tr("Done. No new photos found."):
                tr("Done. %n new photo(s) found.\n"
                   "Photo broom will now collect data from photos.\n"
                   "You can watch progress in 'Tasks' panel."
                   ,
                   "",
                   m_photosFound.size());

            m_notifications.insert(info, INotifications::Type::Info);
            break;
        }
    }
}
