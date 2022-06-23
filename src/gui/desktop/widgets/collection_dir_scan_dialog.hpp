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

#include <QDialog>

#include <core/itasks_view.hpp>
#include <database/idatabase.hpp>
#include "utils/photos_collector.hpp"
#include "inotifications.hpp"

class QLabel;
class Project;

class CollectionDirScanDialog: public QDialog
{
        Q_OBJECT

    public:
        CollectionDirScanDialog(const Project *, Database::IDatabase &, ITasksView &, INotifications &);
        CollectionDirScanDialog(const CollectionDirScanDialog &) = delete;
        ~CollectionDirScanDialog();

        CollectionDirScanDialog& operator=(const CollectionDirScanDialog &) = delete;

        const std::set<QString>& newPhotos() const;

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
        const Project* m_project;
        QLabel* m_info;
        QPushButton* m_button;
        Database::IDatabase& m_database;
        ITasksView& m_tasksView;
        IViewTask* m_progressTask;
        INotifications& m_notifications;
        std::atomic<bool> m_gotPhotos;
        std::atomic<bool> m_gotDBPhotos;

        // slots:
        void buttonPressed();
        void scanDone();
        void performAnalysis();
        //

        void scan();
        void checkIfReady();

        void gotPhoto(const QString &);
        void gotExistingPhotos(const std::vector<Photo::DataDelta> &);
        void updateGui();
};

#endif
