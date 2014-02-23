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


std::map<BaseTagsList, TagNameInfo> BaseTags::m_base_tags( {
                                                { BaseTagsList::Event,  TagNameInfo(Consts::DefaultTags::tag_event,  TagNameInfo::Text) },
                                                { BaseTagsList::Place,  TagNameInfo(Consts::DefaultTags::tag_place,  TagNameInfo::Text) },
                                                { BaseTagsList::Date,   TagNameInfo(Consts::DefaultTags::tag_date,   TagNameInfo::Date) },
                                                { BaseTagsList::Time,   TagNameInfo(Consts::DefaultTags::tag_time,   TagNameInfo::Time) },
                                                { BaseTagsList::People, TagNameInfo(Consts::DefaultTags::tag_people, TagNameInfo::Text) },
                                             } );

TagNameInfo BaseTags::get(const BaseTagsList &item)
{
    auto p = m_base_tags.find(item);
    TagNameInfo result("", TagNameInfo::Invalid);

    if (p != m_base_tags.end())
        result = p->second;

    return result;
}
