/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include <QBuffer>

#include "thumbnail_manager.hpp"

#include <core/constants.hpp>
#include <core/task_executor_traits.hpp>
#include <database/database_executor_traits.hpp>
#include "ithumbnails_cache.hpp"


using namespace std::placeholders;


ThumbnailManager::ThumbnailManager(ITaskExecutor* executor, IThumbnailsGenerator& gen, IThumbnailsCache& cache, Database::IDatabase* db):
    m_tasks(executor, TasksQueue::Mode::Lifo),
    m_cache(cache),
    m_generator(gen),
    m_db(db)
{
}


void ThumbnailManager::fetch(const Photo::Id& id, const QSize& desired_size, const std::function<void(const QImage &)>& callback)
{
    assert(m_db != nullptr);

    // use id as a path to search for thumbnail in cache.
    // TODO: cache needs to be changed to use ids only.

    const QString id_path = QString::number(id);
    const IThumbnailsCache::ThumbnailParameters params(desired_size);

    const QImage cached = find(id_path, params);

    // not cached in memory, search in db (if possible)
    if (cached.isNull())
    {
        m_tasks.addLight(inlineTask("database thumbnail fetch", [this, id, callback, params]()
        {
            // load thumbnail from db
            QByteArray dbThumb = evaluate<QByteArray(Database::IBackend &)>(*m_db, [id](Database::IBackend& backend)
            {
                return backend.getThumbnail(id);
            });

            QImage baseThumbnail;

            // thumbnail not found - generate one and update database cache
            if (dbThumb.isNull())
            {
                // load path to photo
                const Photo::DataDelta photoData = evaluate<Photo::DataDelta(Database::IBackend &)>(*m_db, [id](Database::IBackend& backend)
                {
                    return backend.getPhotoDelta(id, {Photo::Field::Path});
                });

                // generate base thumbnail
                baseThumbnail = evaluate<QImage(), ITaskExecutor>(m_tasks, [=, this]()
                {
                    return m_generator.generate(photoData.get<Photo::Field::Path>(), IThumbnailsCache::ThumbnailParameters(Parameters::databaseThumbnailSize));
                });

                // store thumbnail in db
                QBuffer buf(&dbThumb);
                baseThumbnail.save(&buf, "JPG");

                execute<Database::IDatabase>(*m_db, [id, dbThumb](Database::IBackend& backend)
                {
                    backend.setThumbnail(id, dbThumb);
                });
            }
            else
                baseThumbnail = QImage::fromData(dbThumb, "JPG");

            const QImage thumbnail = evaluate<QImage(), ITaskExecutor>(m_tasks, [baseThumbnail, params, this]()
            {
                return m_generator.generateFrom(baseThumbnail, params);
            });

            callback(thumbnail);
        }));
    }
    else
        callback(cached);
}


std::optional<QImage> ThumbnailManager::fetch(const Photo::Id& id, const QSize& desired_size)
{
    // use id as a path to search for thumbnail in cache.
    // TODO: cache needs to be changed to use ids only.

    const QString id_path = QString::number(id);
    std::optional img = m_cache.find(id_path, IThumbnailsCache::ThumbnailParameters(desired_size));

    return img;
}


void ThumbnailManager::fetch(const QString& path, const QSize& desired_size, const std::function<void(const QImage &)>& callback)
{
    internal_fetch(path, IThumbnailsCache::ThumbnailParameters(desired_size), callback);
}


std::optional<QImage> ThumbnailManager::fetch(const QString& path, const QSize& desired_size)
{
    std::optional img = m_cache.find(path, IThumbnailsCache::ThumbnailParameters(desired_size));

    return img;
}


void ThumbnailManager::setDatabaseCache(Database::IDatabase* db)
{
    m_db = db;
}


QImage ThumbnailManager::find(const QString& path, const IThumbnailsCache::ThumbnailParameters& params)
{
    QImage result;

    const auto cached = m_cache.find(path, params);

    if (cached.has_value())
        result = *cached;

    return result;
}


void ThumbnailManager::cache(const QString& path, const IThumbnailsCache::ThumbnailParameters& params, const QImage& img)
{
    m_cache.store(path, params, img);
}


void ThumbnailManager::generate(const QString& path, const IThumbnailsCache::ThumbnailParameters& params, const std::function<void(const QImage &)>& callback)
{
    runOn(m_tasks, [=, this]
    {
        generate_task(path, params, callback);
    }, "ThumbnailManager::generate");
}

