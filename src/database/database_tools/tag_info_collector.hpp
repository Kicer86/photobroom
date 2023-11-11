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
#include <core/signal_postponer.hpp>

#include "itag_info_collector.hpp"
#include "database_export.h"


namespace Database
{
    struct IDatabase;
}

class DATABASE_EXPORT TagInfoCollector: public ITagInfoCollector
{
    public:
        TagInfoCollector(const ILogger &);
        TagInfoCollector(const TagInfoCollector &) = delete;
        ~TagInfoCollector() = default;

        TagInfoCollector& operator=(const TagInfoCollector &) = delete;

        void set(Database::IDatabase *);

        const std::vector<TagValue>& get(const Tag::Types &) const override;

    private:
        mutable std::map<Tag::Types, std::vector<TagValue>> m_tags;
        mutable std::mutex m_tags_mutex;
        std::unique_ptr<ILogger> m_logger;
        Database::IDatabase* m_database;

        void gotTagValues(const Tag::Types &, const std::vector<TagValue> &);

        void updateAllTags(SignalBlocker::Locker = {});
        void updateValuesFor(const Tag::Types &, SignalBlocker::Locker = {});
};

#endif // TAGINFOCOLLECTOR_H
