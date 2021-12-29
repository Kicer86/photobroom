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

#ifndef IPHOTO_INFO_HPP
#define IPHOTO_INFO_HPP

#include <memory>
#include <string>
#include <vector>

#include <core/tag.hpp>

#include "database_export.h"

#include "photo_data.hpp"

class QString;
class QImage;

class TagDataBase;

struct DATABASE_EXPORT IPhotoInfo
{
    typedef std::shared_ptr<IPhotoInfo> Ptr;
    typedef std::vector<IPhotoInfo::Ptr> List;

    virtual ~IPhotoInfo() = default;

    //data getting
    virtual Photo::Data      data() const = 0;
    virtual QString          getPath() const = 0;
    virtual Tag::TagsList    getTags() const = 0;       // access to tags
    virtual QSize            getGeometry() const = 0;   // get photo geometry
    virtual Photo::Sha256sum getSha256() const = 0;     // Do not call until isSha256Loaded()
    virtual Photo::Id        getID() const = 0;

    //status checking
    virtual bool isFullyInitialized() const = 0;            // returns true if photo fully loaded (all items below are loaded)
    virtual bool isSha256Loaded() const = 0;                // returns true if sha256 is not null
    virtual bool isGeometryLoaded() const = 0;              // returns true if geometry is not null
    virtual bool isExifDataLoaded() const = 0;              // returns true is tags were loaded

    //setting data
    virtual void setData(const Photo::Data &) = 0;          // sets all data
    virtual void setSha256(const Photo::Sha256sum &) = 0;
    virtual void setTags(const Tag::TagsList &) = 0;                  // set tags. tags with empty values (TagValue::type == Empty) will be ignored
    virtual void setTag(const Tag::Types &, const TagValue &) = 0;      // set tag. if TagValue is empty, tag will be removed
    virtual void setGeometry(const QSize &) = 0;
    virtual void setGroup(const GroupInfo &) = 0;

    //flags
    virtual void markFlag(Photo::FlagsE, int) = 0;
    virtual int  getFlag(Photo::FlagsE) const = 0;
};

#endif // IPHOTO_INFO_HPP
