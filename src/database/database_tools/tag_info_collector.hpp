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

#ifndef TAGINFOCOLLECTOR_HPP
#define TAGINFOCOLLECTOR_HPP

#include <mutex>

#include <core/ilogger.hpp>
#include <database/database_tools/signal_mapper.hpp>

#include "itag_info_collector.hpp"
#include "../iphoto_info.hpp"
#include "database_export.h"

namespace Database
{
    struct IDatabase;
}

class DATABASE_EXPORT TagInfoCollector: public ITagInfoCollector
{
    public:
        TagInfoCollector(std::unique_ptr<ILogger>);
        TagInfoCollector(const TagInfoCollector &) = delete;
        ~TagInfoCollector() = default;

        TagInfoCollector& operator=(const TagInfoCollector &) = delete;

        void set(Database::IDatabase *);

        const std::vector<TagValue>& get(const TagTypes &) const override;

    private:
        Database::SignalMapper m_mapper;
        mutable std::map<TagTypes, std::vector<TagValue>> m_tags;
        mutable std::mutex m_tags_mutex;
        std::unique_ptr<ILogger> m_logger;
        Database::IDatabase* m_database;
        int m_observerId;

        void gotTagValues(const TagTypes &, const std::vector<TagValue> &);
        void photoModified(const IPhotoInfo::Ptr &);

        void updateAllTags();
        void updateValuesFor(const TagTypes &);
};

#endif // TAGINFOCOLLECTOR_H
