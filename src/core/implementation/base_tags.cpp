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

#include <configuration/constants.hpp>

namespace
{
    struct DefaultTags
    {
        static const char tag_people[];
        static const char tag_place[];
        static const char tag_time[];
        static const char tag_date[];
        static const char tag_event[];

        static const std::vector<const char *> tags_list;
    };


    const char DefaultTags::tag_people[] = QT_TRANSLATE_NOOP("Default Tag Name", "People");
    const char DefaultTags::tag_place[]  = QT_TRANSLATE_NOOP("Default Tag Name", "Place");
    const char DefaultTags::tag_date[]   = QT_TRANSLATE_NOOP("Default Tag Name", "Date");
    const char DefaultTags::tag_time[]   = QT_TRANSLATE_NOOP("Default Tag Name", "Time");
    const char DefaultTags::tag_event[]  = QT_TRANSLATE_NOOP("Default Tag Name", "Event");

    const std::vector<const char *> DefaultTags::tags_list( { tag_people, tag_place, tag_date, tag_event });
}


std::map<BaseTagsList, TagNameInfo> BaseTags::m_base_tags(
{
    { BaseTagsList::Event,  TagNameInfo(DefaultTags::tag_event,  TagNameInfo::Text) },
    { BaseTagsList::Place,  TagNameInfo(DefaultTags::tag_place,  TagNameInfo::Text) },
    { BaseTagsList::Date,   TagNameInfo(DefaultTags::tag_date,   TagNameInfo::Date) },
    { BaseTagsList::Time,   TagNameInfo(DefaultTags::tag_time,   TagNameInfo::Time) },
    { BaseTagsList::People, TagNameInfo(DefaultTags::tag_people, TagNameInfo::Text) },
} );


std::vector<TagNameInfo> BaseTags::collectTags()
{
    std::vector<TagNameInfo> result;

    for(const auto it: m_base_tags)
        result.push_back(it.second);

    return result;
}


TagNameInfo BaseTags::get(const BaseTagsList &item)
{
    auto p = m_base_tags.find(item);
    TagNameInfo result("", TagNameInfo::Invalid);

    if (p != m_base_tags.end())
        result = p->second;

    return result;
}


const std::vector<TagNameInfo>& BaseTags::getAll()
{
    static std::vector<TagNameInfo> tags = collectTags();

    return tags;
}

