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

#include "photos_collector.hpp"

#include <cassert>

#include <photos_crawler/default_analyzers/file_analyzer.hpp>
#include <photos_crawler/photo_crawler.hpp>
#include <photos_crawler/default_filesystem_scanners/filesystemscanner.hpp>
#include <project_utils/project.hpp>


struct PhotosCollector::Data
{
    std::function<void(const QString &)> m_callback;
    ITasksView* m_tasksView;
    std::unique_ptr<PhotoCrawler> m_crawler;
    const Project* m_project;

    Data(): m_callback(), m_tasksView(nullptr), m_crawler(nullptr)
    {

    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;
};


PhotosCollector::PhotosCollector(const Project* project, QObject* p): QObject(p), m_data(new Data)
{
    m_data->m_project = project;
}


PhotosCollector::~PhotosCollector()
{

}


void PhotosCollector::collect(const QString& path, const std::function<void(const QString &)>& callback)
{
    stop();

    m_data->m_callback = callback;

    auto analyzer = std::make_unique<FileAnalyzer>();
    auto scanner = std::make_unique<FileSystemScanner>();

    const ProjectInfo& info = m_data->m_project->getProjectInfo();
    const QString& internals = info.getInternalLocation();
    const QStringList to_ignore { internals };

    scanner->ignorePaths(to_ignore);

    m_data->m_crawler = std::make_unique<PhotoCrawler>( std::move(scanner), std::move(analyzer) );
    m_data->m_crawler->crawl(path, this);
}


void PhotosCollector::stop()
{
    if (m_data->m_crawler.get() != nullptr)
        m_data->m_crawler->stop();
}


void PhotosCollector::found(const QString& path)
{
    m_data->m_callback(path);
}
