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
    static std::map<BaseTagsList, TagNameInfo> base_tags(
    {
        { BaseTagsList::Event,  TagNameInfo("Event",  TagType::Text, tr("Event")) },
        { BaseTagsList::Place,  TagNameInfo("Place",  TagType::Text, tr("Place")) },
        { BaseTagsList::Date,   TagNameInfo("Date",   TagType::Date, tr("Date")) },
        { BaseTagsList::Time,   TagNameInfo("Time",   TagType::Time, tr("Time")) },
        { BaseTagsList::People, TagNameInfo("People", TagType::List, tr("People")) },
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
    TagNameInfo result;

    if (p != tags.end())
        result = p->second;

    return result;
}


const std::vector<TagNameInfo>& BaseTags::getAll()
{
    static std::vector<TagNameInfo> tags = collectTags();

    return tags;
}

