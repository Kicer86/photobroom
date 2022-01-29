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

#ifndef TAGS_OPERATOR_HPP
#define TAGS_OPERATOR_HPP

#include <vector>

#include <database/iphoto_info.hpp>

#include "itags_operator.hpp"

class TagsOperator: public ITagsOperator
{
    public:
        TagsOperator();

        void operateOn(const std::vector< IPhotoInfo::Ptr >&) override;

        Tag::TagsList getTags() const override;

        void setTag(const Tag::Types &, const TagValue &) override;
        void setTags(const Tag::TagsList &) override;
        void insert(const Tag::Types& name, const TagValue & ) override;

    private:
        std::vector<IPhotoInfo::Ptr> m_photos;
};

#endif // TAGS_OPERATOR_HPP
