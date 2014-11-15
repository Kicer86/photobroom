/*
 * Tool for updating Photo's tags
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

#include "tag_updater.hpp"

TagUpdater::TagUpdater(const IPhotoInfo::Ptr& photo): m_photoInfo(photo)
{

}


void TagUpdater::clear()
{
    Tag::TagsList tags;
    m_photoInfo->setTags(tags);
}


void TagUpdater::setTags(const Tag::TagsList& tags)
{
    m_photoInfo->setTags(tags);
}


void TagUpdater::setTag(const TagNameInfo& name, const TagValue& values)
{
    auto tags = m_photoInfo->accessTags();
    tags.get()[name] = values;
}


Tag::TagsList TagUpdater::getTags() const
{
    Tag::TagsList tags = m_photoInfo->getTags();
    return tags;
}
