/*
 * Utility for photos collecting.
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

#include "photos_collector.hpp"

#include <cassert>

#include <QProgressBar>

#include <photos_crawler/photo_crawler_builder.hpp>

#include "widgets/staged_photos_data_model.hpp"


PhotosReceiver::PhotosReceiver(const std::function<void(const QString &)>& callback): m_callback(callback)
{

}


void PhotosReceiver::found(const QString &path)
{
    m_callback(path);
}


///////////////////////////////////////////////////////////////////////////////


struct PhotosCollector::Data
{
    StagedPhotosDataModel* m_model;
    ITasksView* m_tasksView;
    std::unique_ptr<IPhotoCrawler> m_crawler;
    std::unique_ptr<PhotosReceiver> m_receiver;

    Data(): m_model(nullptr), m_tasksView(nullptr), m_crawler(nullptr), m_receiver(nullptr)
    {

    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;
};


PhotosCollector::PhotosCollector(QObject* p): QObject(p), m_data(new Data)
{
}


PhotosCollector::~PhotosCollector()
{

}


void PhotosCollector::workIsDone()
{
    emit finished();
}


void PhotosCollector::collect(const QString& path, const std::function<void(const QString &)>& callback)
{
    m_data->m_receiver = std::make_unique<PhotosReceiver>(callback);

    connect(m_data->m_receiver.get(), &PhotosReceiver::finished, this, &PhotosCollector::workIsDone);

    m_data->m_crawler = PhotoCrawlerBuilder().build();
    m_data->m_crawler->crawl(path, m_data->m_receiver.get());
}


void PhotosCollector::stop()
{
    m_data->m_crawler->stop();
}

