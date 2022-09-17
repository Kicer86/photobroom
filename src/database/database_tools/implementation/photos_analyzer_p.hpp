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


#include <QObject>

#include <core/accumulative_queue.hpp>
#include <core/itasks_view.hpp>
#include <core/iview_task.hpp>
#include <core/media_information.hpp>
#include <core/observable_task_executor.hpp>
#include <core/task_executor_utils.hpp>
#include <database/idatabase.hpp>
#include <database/database_queue.hpp>


class PhotosAnalyzerImpl: public QObject
{
        Q_OBJECT

    public:
        PhotosAnalyzerImpl(ICoreFactoryAccessor *, Database::IDatabase &);
        PhotosAnalyzerImpl(const PhotosAnalyzerImpl&) = delete;
        PhotosAnalyzerImpl& operator=(const PhotosAnalyzerImpl&) = delete;

        ~PhotosAnalyzerImpl();

        void set(ITasksView* tasksView);
        void stop();

    private:
        ObservableTaskExecutor<TasksQueue> m_taskQueue;
        MediaInformation m_mediaInformation;
        QMetaObject::Connection m_backendConnection;
        std::weak_ptr<Database::DatabaseQueue> m_storageQueue;
        std::mutex m_storageMutex;
        Database::IDatabase& m_database;
        ITasksView* m_tasksView;
        IViewTask* m_viewTask;
        int m_maxPhotos;

        void addPhotos(const std::vector<Photo::Id> &);
        void finishProgressBar();
};

#endif
