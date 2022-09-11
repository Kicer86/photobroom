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

#ifndef PHOTOSCOLLECTOR_HPP
#define PHOTOSCOLLECTOR_HPP

#include <functional>
#include <memory>

#include <QObject>

#include <photos_crawler/iphoto_crawler.hpp>

class QString;

struct ITasksView;
class Project;

class PhotosCollector: public QObject, public IMediaNotification
{
        Q_OBJECT

    public:
        PhotosCollector(const Project &, QObject * = nullptr);
        PhotosCollector(const PhotosCollector& other) = delete;
        ~PhotosCollector();
        PhotosCollector& operator=(const PhotosCollector& other) = delete;

        void collect(const std::function<void(const QString &)> &);
        void stop();

    signals:
        // IMediaNotification:
        void finished() override;

    private:
        std::function<void(const QString &)> m_callback;
        ITasksView* m_tasksView = nullptr;
        std::unique_ptr<IPhotoCrawler> m_crawler;
        const Project& m_project;

        // IMediaNotification:
        void found(const QString& path) override;
};

#endif // PHOTOSCOLLECTOR_HPP
