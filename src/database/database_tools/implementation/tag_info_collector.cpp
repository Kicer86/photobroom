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

#include "../tag_info_collector.hpp"

#include <memory>

#include <core/base_tags.hpp>
#include "idatabase.hpp"


TagInfoCollector::TagInfoCollector(const ILogger& logger)
    : m_logger(logger.subLogger("TagInfoCollector"))
    , m_database(nullptr)
{

}


void TagInfoCollector::set(Database::IDatabase* db)
{
    m_database = db;

    // TODO: ADatabaseSignals doesn't emit signal when tags are changed.
    // It would require improvements in backend (#10 and maybe #180), so for now listen for photo modifications.
    // Github issue: #183

    if (m_database != nullptr)
        blocked_connect(&db->backend(), &Database::IBackend::photosModified,
                        this, &TagInfoCollector::updateAllTags,
                        std::chrono::seconds(15))->notify();
}


const std::vector<TagValue>& TagInfoCollector::get(const Tag::Types& info) const
{
    std::lock_guard<std::mutex> lock(m_tags_mutex);
    return m_tags[info];
}


void TagInfoCollector::gotTagValues(const Tag::Types& tagType, const std::vector<TagValue>& values)
{
    std::unique_lock<std::mutex> lock(m_tags_mutex);
    m_tags[tagType] = values;
    lock.unlock();

    emit setOfValuesChanged(tagType);
}


void TagInfoCollector::updateAllTags(SignalBlocker::Locker locker)
{
    m_logger->trace("updating all tags");
    auto tagNames = BaseTags::getAll();

    for(const Tag::Types& baseTagName: tagNames)
        updateValuesFor(baseTagName, locker);
}


void TagInfoCollector::updateValuesFor(const Tag::Types& tagType, SignalBlocker::Locker locker)
{
    if (m_database != nullptr)
    {
        using namespace std::placeholders;
        auto result = std::bind(&TagInfoCollector::gotTagValues, this, _1, _2);
        m_database->exec([tagType, result, locker](Database::IBackend& backend)
            {
                const auto values = backend.listTagValues(tagType, {});
                result(tagType, values);
            },
            "TagInfoCollector: fetch tag values"
        );
    }
}
