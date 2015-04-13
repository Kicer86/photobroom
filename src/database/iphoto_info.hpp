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

#include "database_export.h"
#include <OpenLibrary/putils/ts_resource.hpp>

class QString;
class QPixmap;

class TagDataBase;

struct IPhotoInfo
{
    typedef std::shared_ptr<IPhotoInfo> Ptr;
    typedef std::deque<IPhotoInfo::Ptr> List;
    typedef std::string Sha256sum;

    struct DATABASE_EXPORT Id
    {
        typedef int type;

        Id();
        explicit Id(type);
        Id(const Id &) = default;

        Id& operator=(const Id &) = default;
        operator type() const;
        bool operator!() const;
        bool valid() const;
        type value() const;

    private:
        type m_value;
        bool m_valid;
    };

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

    enum class FlagsE
    {
        StagingArea,
        ExifLoaded,
        Sha256Loaded,
        ThumbnailLoaded,
    };

    virtual ~IPhotoInfo() {}

    //data getting
    virtual const QString& getPath() const = 0;
    virtual const Tag::TagsList& getTags() const = 0;       // access to tags
    virtual const QPixmap& getThumbnail() const = 0;        // a temporary thumbnail may be returned when final one is not yet generated.
    virtual const Sha256sum& getSha256() const = 0;         // Do not call until isSha256Loaded()
    virtual Id getID() const = 0;

    //status checking
    virtual bool isFullyInitialized() const = 0;            // returns true if photo fully loaded (all items below are loaded)
    virtual bool isSha256Loaded() const = 0;                // returns true if sha256 is not null
    virtual bool isThumbnailLoaded() const = 0;             // returns true if thumbnail is loaded
    virtual bool isExifDataLoaded() const = 0;              // returns true is tags were loaded

    //observers
    virtual void registerObserver(IObserver *) = 0;
    virtual void unregisterObserver(IObserver *) = 0;

    //data initializing
    virtual void initSha256(const Sha256sum &) = 0;
    virtual void initThumbnail(const QPixmap &) = 0;
    virtual void initID(const Id &) = 0;

    //setting data
    virtual ol::ThreadSafeResource<Tag::TagsList>::Accessor accessTags() = 0;   // gives exclusive access to tags so they can be modified in conveniant fashion
    virtual void setTags(const Tag::TagsList &) = 0;        //set tags

    //flags
    virtual void markFlag(FlagsE, int) = 0;
    virtual int  getFlag(FlagsE) const = 0;
};


struct PhotoInfoIdHash
{
    std::size_t operator()(const ::IPhotoInfo::Id& key) const
    {
        return std::hash<IPhotoInfo::Id::type>()(key.value());
    }
};


#endif // IPHOTO_INFO_HPP
