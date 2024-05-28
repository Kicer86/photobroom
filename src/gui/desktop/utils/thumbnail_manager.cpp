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

namespace
{
    constexpr auto BlobType = "thumbnail";
}

using namespace std::placeholders;


ThumbnailManager::ThumbnailManager(ITaskExecutor& executor, IThumbnailsGenerator& gen, IThumbnailsCache& cache, const ILogger& logger, Database::IDatabase* db):
    m_tasks(executor, TasksQueue::Mode::Lifo),
    m_logger(logger.subLogger("ThumbnailManager")),
    m_cache(cache),
    m_generator(gen),
    m_db(db)
{
}


ThumbnailManager::~ThumbnailManager()
{
    m_callbackCtrl.invalidate();
}


void ThumbnailManager::fetch(const Photo::Id& id, const QSize& desired_size, const std::function<void(const QImage &)>& callback)
{
    std::lock_guard<std::mutex> _(m_cacheMutex);

    assert(id.valid());
    assert(desired_size.isEmpty() == false);

    const IThumbnailsCache::ThumbnailParameters params(desired_size);
    const QImage cached = find(id, params);

    // not cached in memory, search in db (if possible)
    if (cached.isNull() && m_db)
    {
        auto task = m_callbackCtrl.make_safe_callback([=, this]()
        {
            QByteArray dbThumb;

            if (m_db)                   // load thumbnail from db
                dbThumb = evaluate(*m_db, [id](Database::IBackend& backend)
                {
                    return backend.readBlob(id, BlobType);
                });

            QImage baseThumbnail;

            // thumbnail not found - generate one and update database cache
            if (dbThumb.isNull())
            {
                // load path to photo
                const Photo::DataDelta photoData = evaluate(*m_db, [id](Database::IBackend& backend)
                {
                    return backend.getPhotoDelta<Photo::Field::Path>(id);
                });

                // generate base thumbnail
                baseThumbnail = m_generator.generate(photoData.get<Photo::Field::Path>(), IThumbnailsCache::ThumbnailParameters(Parameters::databaseThumbnailSize));

                if (baseThumbnail.isNull())
                    m_logger->error(QString("Generator returned empty thumbnail for %1").arg(photoData.get<Photo::Field::Path>()));
                else
                {
                    // store thumbnail in db
                    QBuffer buf(&dbThumb);
                    baseThumbnail.save(&buf, "JPG");

                    execute<Database::IDatabase>(*m_db, [id, dbThumb](Database::IBackend& backend)
                    {
                        backend.writeBlob(id, BlobType, dbThumb);
                    });
                }
            }
            else
            {
                baseThumbnail = QImage::fromData(dbThumb, "JPG");

                if (baseThumbnail.isNull())
                    m_logger->error(QString("Error when loading JPG file from raw data for photo %1").arg(id.value()));
            }

            // handle errors in generation
            if (baseThumbnail.isNull())
                baseThumbnail.load(":/gui/error.svg");

            // resize base thumbnail to required size
            const QImage thumbnail = m_generator.generateFrom(baseThumbnail, params);

            cache(id, params, thumbnail);
            callback(thumbnail);
        });

        m_tasks.add(inlineTask("database thumbnail fetch", task));
    }
    else
        callback(cached);
}


std::optional<QImage> ThumbnailManager::fetch(const Photo::Id& id, const QSize& desired_size)
{
    std::lock_guard<std::mutex> _(m_cacheMutex);

    std::optional img = m_cache.find(id, IThumbnailsCache::ThumbnailParameters(desired_size));

    return img;
}


void ThumbnailManager::setDatabaseCache(Database::IDatabase* db)
{
    std::lock_guard<std::mutex> _(m_cacheMutex);

    m_callbackCtrl.invalidate();
    m_db = db;
    m_cache.clear();
}


QImage ThumbnailManager::find(const Photo::Id& id, const IThumbnailsCache::ThumbnailParameters& params)
{
    QImage result;

    const auto cached = m_cache.find(id, params);

    if (cached.has_value())
        result = *cached;

    return result;
}


void ThumbnailManager::cache(const Photo::Id& id, const IThumbnailsCache::ThumbnailParameters& params, const QImage& img)
{
    m_cache.store(id, params, img);
}
