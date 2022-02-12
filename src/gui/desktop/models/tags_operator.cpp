/*
 * Photo Broom - photos management tool.
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

#include "tags_operator.hpp"

#include <QObject>


TagsOperator::TagsOperator()
{

}


void TagsOperator::operateOn(const std::vector<Photo::Data>& photos)
{
    m_diffs.clear();

    for(const auto& photo: photos)
    {
        Photo::DataDelta delta;
        delta.insert<Photo::Field::Tags>(photo.tags);

        m_diffs.push_back(delta);
    }
}


Tag::TagsList TagsOperator::getTags() const
{
    struct
    {
        bool operator()(const std::pair<Tag::Types, TagValue> &a,
                        const std::pair<Tag::Types, TagValue> &b) const
        {
            return std::tie(a.first, a.second) < std::tie(b.first, b.second);
        }

    } tagsComparer;

    Tag::TagsList commonTags;
    bool first = true;

    for (const auto& photo: m_diffs)
    {
        const Tag::TagsList& tags = photo.get<Photo::Field::Tags>();

        if (first)
            commonTags = tags, first = false;
        else
        {
            Tag::TagsList intersection;
            std::set_intersection(commonTags.begin(), commonTags.end(),
                                  tags.begin(), tags.end(),
                                  std::inserter(intersection, intersection.end()), tagsComparer);

            commonTags = intersection;
        }
    }

    return commonTags;
}


void TagsOperator::setTag(const Tag::Types& name, const TagValue& values)
{
    for(auto& delta: m_diffs)
    {
        Tag::TagsList& tags = delta.get<Photo::Field::Tags>();
        tags[name] = values;
    }
}


void TagsOperator::setTags(const Tag::TagsList& tags)
{
    for(auto& delta: m_diffs)
        delta.insert<Photo::Field::Tags>(tags);
}


void TagsOperator::insert(const Tag::Types& name, const TagValue& value)
{
    //find tag for given name
    Tag::TagsList tags = getTags();
    bool updated = false;

    for(const auto& info: tags) // TODO: use std::find_if
    {
        if (info.first == name)
        {
            const bool differs = info.second != value;

            if (differs)
                setTag(name, value);

            updated = true;
            break;
        }
    }

    if (updated == false)
        setTag(name, value);
}
