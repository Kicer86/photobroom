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

#ifndef TAGUPDATER_H
#define TAGUPDATER_H

#include <core/tag.hpp>
#include <database/iphoto_info.hpp>

class TagUpdater : public ITagData
{
    public:
        TagUpdater(const IPhotoInfo::Ptr &);
        TagUpdater(const TagUpdater &) = delete;
        virtual ~TagUpdater();

        TagUpdater& operator=(const TagUpdater &) = delete;

        virtual bool isValid() const override;
        virtual void clear() override;
        virtual void setTags(const Tag::TagsList& ) override;
        virtual void setTag(const TagNameInfo& name, const TagValueInfo& value) override;
        virtual void setTag(const TagNameInfo& name, const Tag::ValuesSet& values) override;
        virtual Tag::TagsList getTags() const override;

    private:
        IPhotoInfo::Ptr m_photoInfo;
};

#endif // TAGUPDATER_H
