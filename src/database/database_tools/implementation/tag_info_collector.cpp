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

#include "idatabase.hpp"


TagInfoCollector::TagInfoCollector(): m_tags(), m_tags_mutex(), m_observers(), m_database(nullptr), m_observerId(0)
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
    connect(m_database->notifier(), &Database::ADatabaseSignals::photoModified, this, &TagInfoCollector::photoModified);

    updateAllTags();
}


const std::set<TagValue>& TagInfoCollector::get(const TagNameInfo& info) const
{
    std::lock_guard<std::mutex> lock(m_tags_mutex);
    return m_tags[info];
}


int TagInfoCollector::registerChangeObserver(const std::function<void(const TagNameInfo &)>& observer)
{
    const int id = m_observerId++;

    m_observers[id] = observer;

    return id;
}


void TagInfoCollector::unregisterChangeObserver(int id)
{
    auto it = m_observers.find(id);
    assert(it != m_observers.end());

    m_observers.erase(it);
}


void TagInfoCollector::gotTagNames(const std::deque<TagNameInfo>& names)
{
    for( const auto& name: names)
        updateValuesFor(name);
}


void TagInfoCollector::gotTagValues(const TagNameInfo& name, const std::deque<TagValue>& values)
{
    std::set<TagValue> tagValues;

    for(const TagValue& v: values)
        tagValues.insert(v);

    assert(values.size() == tagValues.size());

    std::lock_guard<std::mutex> lock(m_tags_mutex);
    m_tags[name].swap(tagValues);

    for(auto& observer: m_observers)
        observer.second(name);
}


void TagInfoCollector::photoModified(const IPhotoInfo::Ptr& photoInfo)
{
    const Tag::TagsList tags = photoInfo->getTags();

    for(const auto& tag: tags)
        updateValuesFor(tag.first);
}


void TagInfoCollector::updateAllTags()
{
    using namespace std::placeholders;
    auto result = std::bind(&TagInfoCollector::gotTagNames, this, _1);
    m_database->listTagNames(result);
}


void TagInfoCollector::updateValuesFor(const TagNameInfo& name)
{
    using namespace std::placeholders;
    auto result = std::bind(&TagInfoCollector::gotTagValues, this, _1, _2);
    m_database->listTagValues(name, result);
}
