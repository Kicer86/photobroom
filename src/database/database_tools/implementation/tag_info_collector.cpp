/*
 * <one line to give the program's name and a brief idea of what it does.>
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


TagInfoCollector::TagInfoCollector(): m_tags(), m_tags_mutex(), m_database(nullptr), m_observerId(0)
{

}


TagInfoCollector::~TagInfoCollector()
{

}


void TagInfoCollector::set(Database::IDatabase* db)
{
    m_database = db;

    // TODO: ADatabaseSignals doesn't emit signal when tags are changed.
    // It would require improvements in backend (#10 and maybe #180), so for now listen for photo modifications.
    // Github issue: #183

    if (m_database != nullptr)
    {
        connect(m_database->notifier(), &Database::ADatabaseSignals::photoModified,
                this,                   &TagInfoCollector::photoModified);

        updateAllTags();
    }
}


const std::vector<TagValue>& TagInfoCollector::get(const TagNameInfo& info) const
{
    std::lock_guard<std::mutex> lock(m_tags_mutex);
    return m_tags[info];
}


void TagInfoCollector::gotTagValues(const TagNameInfo& name, const std::vector<TagValue>& values)
{
    std::unique_lock<std::mutex> lock(m_tags_mutex);
    m_tags[name] = values;
    lock.unlock();

    emit setOfValuesChanged(name);
}


void TagInfoCollector::photoModified(const IPhotoInfo::Ptr& photoInfo)
{
    const Tag::TagsList tags = photoInfo->getTags();

    // TODO: it would be nice to ask database about current set of values for all photos
    // but it is very noisy when many photos are being updated (newly added for example).
    // For now we just read all tags from changed photos and append their values.

    std::unique_lock<std::mutex> lock(m_tags_mutex);

    for(const auto& tag: tags)
    {
        const TagNameInfo& tagNameInfo = tag.first;
        const TagValue& tagValue = tag.second;

        std::vector<TagValue>& values = m_tags[tagNameInfo];
        auto found = std::find(values.begin(), values.end(), tagValue);

        if (found == values.end())
            values.emplace_back(tagValue);
    }

    lock.unlock();

    // send notifications
    for(const auto& tag: tags)
    {
        const TagNameInfo& tagNameInfo = tag.first;

        emit setOfValuesChanged(tagNameInfo);
    }
}


void TagInfoCollector::updateAllTags()
{
    auto tagNames = BaseTags::getAll();

    for(const BaseTagsList& baseTagName: tagNames)
    {
        const TagNameInfo tagName(baseTagName);
        updateValuesFor(tagName);
    }
}


void TagInfoCollector::updateValuesFor(const TagNameInfo& name)
{
    if (m_database != nullptr)
    {
        using namespace std::placeholders;
        auto result = std::bind(&TagInfoCollector::gotTagValues, this, _1, _2);
        m_database->listTagValues(name, result);
    }
}
