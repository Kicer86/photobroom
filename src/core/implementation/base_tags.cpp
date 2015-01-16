/*
 * Set of base tags
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

#include "base_tags.hpp"

#include <QObject>

const std::map< BaseTagsList, TagNameInfo >& BaseTags::getBaseList()
{
    static const QString tag_people = QObject::tr("People", "Default Tag Name");
    static const QString tag_place  = QObject::tr("Place",  "Default Tag Name");
    static const QString tag_date   = QObject::tr("Date",   "Default Tag Name");
    static const QString tag_time   = QObject::tr("Time",   "Default Tag Name");
    static const QString tag_event  = QObject::tr("Event",  "Default Tag Name");

    static std::map<BaseTagsList, TagNameInfo> base_tags(
    {
        { BaseTagsList::Event,  TagNameInfo(tag_event,  TagNameInfo::Text) },
        { BaseTagsList::Place,  TagNameInfo(tag_place,  TagNameInfo::Text) },
        { BaseTagsList::Date,   TagNameInfo(tag_date,   TagNameInfo::Date) },
        { BaseTagsList::Time,   TagNameInfo(tag_time,   TagNameInfo::Time) },
        { BaseTagsList::People, TagNameInfo(tag_people, TagNameInfo::Text) },
    });

    return base_tags;
}



std::vector<TagNameInfo> BaseTags::collectTags()
{
    std::vector<TagNameInfo> result;

    const std::map<BaseTagsList, TagNameInfo>& tags = getBaseList();
    for (const auto & it: tags)
        result.push_back(it.second);

    return result;
}


TagNameInfo BaseTags::get(const BaseTagsList& item)
{
    const std::map<BaseTagsList, TagNameInfo>& tags = getBaseList();
    auto p = tags.find(item);
    TagNameInfo result("", TagNameInfo::Invalid);

    if (p != tags.end())
        result = p->second;

    return result;
}


const std::vector<TagNameInfo>& BaseTags::getAll()
{
    static std::vector<TagNameInfo> tags = collectTags();

    return tags;
}

