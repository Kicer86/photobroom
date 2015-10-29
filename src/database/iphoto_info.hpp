/*
 * Interface for PhotoInfo
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
#include <deque>

#include <core/tag.hpp>

#include <OpenLibrary/putils/ts_resource.hpp>

#include "database_export.h"

#include "photo_data.hpp"

class QString;
class QImage;

class TagDataBase;

struct IPhotoInfo
{
    typedef std::shared_ptr<IPhotoInfo> Ptr;
    typedef std::deque<IPhotoInfo::Ptr> List;

    struct IObserver
    {
        virtual ~IObserver() {}
        virtual void photoUpdated(IPhotoInfo *) = 0;
    };

    struct DATABASE_EXPORT Flags
    {
        //information
        bool stagingArea;

        //related to data loading
        bool exifLoaded;
        bool sha256Loaded;
        bool thumbnailLoaded;

        Flags();
    };

    virtual ~IPhotoInfo() {}

    virtual Database::PhotoData data() const = 0;

    //data getting
    virtual const QString& getPath() const = 0;
    virtual const Tag::TagsList& getTags() const = 0;          // access to tags
    virtual const QImage& getThumbnail() const = 0;            // a temporary thumbnail may be returned when final one is not yet generated.
    virtual const Database::Sha256sum& getSha256() const = 0;  // Do not call until isSha256Loaded()
    virtual Database::Id getID() const = 0;

    //status checking
    virtual bool isFullyInitialized() const = 0;            // returns true if photo fully loaded (all items below are loaded)
    virtual bool isSha256Loaded() const = 0;                // returns true if sha256 is not null
    virtual bool isThumbnailLoaded() const = 0;             // returns true if thumbnail is loaded
    virtual bool isExifDataLoaded() const = 0;              // returns true is tags were loaded

    //observers
    virtual void registerObserver(IObserver *) = 0;
    virtual void unregisterObserver(IObserver *) = 0;

    //data initializing
    virtual void initSha256(const Database::Sha256sum &) = 0;
    virtual void initThumbnail(const QImage &) = 0;
    virtual void initID(const Database::Id &) = 0;

    //setting data
    virtual ol::ThreadSafeResource<Tag::TagsList>::Accessor accessTags() = 0;   // gives exclusive access to tags so they can be modified in conveniant fashion
    virtual void setTags(const Tag::TagsList &) = 0;        //set tags

    //flags
    virtual void markFlag(Database::FlagsE, int) = 0;
    virtual int  getFlag(Database::FlagsE) const = 0;

    // other
    virtual void invalidate() = 0;                          // mark photo as dropped (with no equivalent in db)
    virtual bool isValid() = 0;
};


#endif // IPHOTO_INFO_HPP
