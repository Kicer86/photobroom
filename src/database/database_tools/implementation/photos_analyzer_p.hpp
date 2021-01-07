/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef PHOTOSANALYZER_PRIVATE_HPP
#define PHOTOSANALYZER_PRIVATE_HPP


#include <thread>
#include <condition_variable>
#include <mutex>

#include <QTimer>

#include <core/itasks_view.hpp>
#include <core/iview_task.hpp>
#include <database/idatabase.hpp>

#include "photo_info_updater.hpp"


class PhotosAnalyzerImpl: public QObject
{
        Q_OBJECT

    public:
        PhotosAnalyzerImpl(ICoreFactoryAccessor *, Database::IDatabase *);
        PhotosAnalyzerImpl(const PhotosAnalyzerImpl&) = delete;
        PhotosAnalyzerImpl& operator=(const PhotosAnalyzerImpl&) = delete;

        ~PhotosAnalyzerImpl();

        void set(ITasksView* tasksView);

        Database::IDatabase* getDatabase();

        void addPhoto(const IPhotoInfo::Ptr& photo);
        void stop();

    private:
        PhotoInfoUpdater m_updater;
        QTimer m_timer;
        std::vector<Photo::Id> m_photosToUpdate;
        Database::IDatabase* m_database;
        ITasksView* m_tasksView;
        IViewTask* m_viewTask;
        int m_maxTasks;

        void setupRefresher();
        void refreshView();
        void newPhotosAdded(const std::vector<IPhotoInfo::Ptr> &);
        void addPhotos(const std::vector<Photo::Id> &);
};

#endif // PHOTOSANALYZER_PRIVATE_HPP

