/*
 * Tool for generating thumbnails
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

#ifndef THUMBNAILGENERATOR_HPP
#define THUMBNAILGENERATOR_HPP

#include "athumbnail_manager.hpp"

#include "exif_reader_factory.hpp"
#include "task_executor_utils.hpp"
#include "core_export.h"


struct IConfiguration;
struct IExifReaderFactory;
struct ILogger;


class CORE_EXPORT ThumbnailGenerator: public AThumbnailGenerator
{
    public:
        ThumbnailGenerator(ITaskExecutor *, ILogger *, IConfiguration *);
        ThumbnailGenerator(const ThumbnailGenerator &) = delete;
        ~ThumbnailGenerator();

        ThumbnailGenerator& operator=(const ThumbnailGenerator &) = delete;

        void dismissPendingTasks();

        // IThumbnailGenerator:
        //void generateThumbnail(const ThumbnailInfo &, const Callback &) const override;
        void run(const QString & , int , std::unique_ptr<ICallback> ) override;

    private:
        std::unique_ptr<TasksQueue> m_tasks;
        ILogger* m_logger;
        mutable ExifReaderFactory m_exifReaderFactory;
        IConfiguration* m_configuration;

        struct FromImageTask;
        friend struct FromImageTask;

        struct FromVideoTask;
        friend struct FromVideoTask;
};

#endif // THUMBNAILGENERATOR_HPP
