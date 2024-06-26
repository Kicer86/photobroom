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

#ifndef COLLECTIONDIRSCANDIALOG_HPP
#define COLLECTIONDIRSCANDIALOG_HPP

#include <atomic>
#include <set>

#include <core/itasks_view.hpp>
#include <database/idatabase.hpp>
#include "utils/photos_collector.hpp"
#include "inotifications.hpp"


class CollectionScanner: public QObject
{
        Q_OBJECT

    public:
        CollectionScanner(const Project &, ITasksView &, INotifications &);
        CollectionScanner(const CollectionScanner &) = delete;
        ~CollectionScanner();

        CollectionScanner& operator=(const CollectionScanner &) = delete;

        void scan();

    signals:
        void scanFinished() const;

    private:
        using PhotoPaths = Photo::ExplicitDelta<Photo::Field::Path>;
        PhotosCollector m_collector;
        std::vector<Photo::DataDelta> m_diskPhotos;
        std::vector<PhotoPaths> m_dbPhotos;
        std::vector<PhotoPaths> m_missingPhotos;
        const Project& m_project;
        Database::IDatabase& m_database;
        ITasksView& m_tasksView;
        IViewTask* m_progressTask;
        INotifications& m_notifications;
        std::atomic<bool> m_gotPhotos;
        std::atomic<bool> m_gotDBPhotos;

        // slots:
        void diskScanDone();
        void performAnalysis();
        //

        void checkIfReady();

        void gotDiskPhoto(const QString &);
        void gotDBPhotos(const std::vector<PhotoPaths> &, const std::vector<PhotoPaths> &);
        void addNotification(std::size_t, std::size_t, std::size_t);
};

#endif
