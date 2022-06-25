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
        enum class State
        {
            Canceled,
            Scanning,
            Analyzing,
            Done,
        };

        PhotosCollector m_collector;
        std::set<QString> m_photosFound;
        std::vector<Photo::DataDelta> m_dbPhotos;
        State m_state;
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

        void gotPhoto(const QString &);
        void gotExistingPhotos(const std::vector<Photo::DataDelta> &);
        void updateGui();
};

#endif
