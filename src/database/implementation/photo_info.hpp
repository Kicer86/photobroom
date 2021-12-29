
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>
#include <cstdint>
#include <string>

#include <QString>

#include "iphoto_info.hpp"

#include "database_export.h"

class QPixmap;

namespace Database
{
    struct IDatabase;
}

namespace Photo
{
    struct Data;
}

class PhotoInfo final: public IPhotoInfo
{
    public:
        PhotoInfo(const Photo::Data &, Database::IDatabase *);
        PhotoInfo(const PhotoInfo &) = delete;
        virtual ~PhotoInfo();

        PhotoInfo& operator=(const PhotoInfo &) = delete;

        //data getting
        Photo::Data      data() const override;
        QString          getPath() const override;
        Tag::TagsList    getTags() const override;           // a access to tags
        QSize            getGeometry() const override;       // get photo's geometry
        Photo::Sha256sum getSha256() const override;         // Do not call until isSha256Loaded()
        Photo::Id        getID() const override;

        //status checking
        bool isFullyInitialized() const override;            // returns true if photo fully loaded (all items below are loaded)
        bool isSha256Loaded() const override;                // returns true if sha256 is not null
        bool isGeometryLoaded() const override;              // returns true if photo's geometry was loaded
        bool isExifDataLoaded() const override;              // returns true is exif for this photo was read

        //set data
        void setData(const Photo::Data & ) override;
        void setSha256(const Photo::Sha256sum &) override;
        void setGeometry(const QSize &) override;
        void setTags(const Tag::TagsList &) override;
        void setTag(const Tag::Types &, const TagValue &) override;
        void setGroup(const GroupInfo &) override;

        //flags
        void markFlag(Photo::FlagsE, int) override;
        int getFlag(Photo::FlagsE) const override;

    private:
        mutable ol::ThreadSafeResource<Photo::Data> m_data;
        Database::IDatabase* m_storekeeper;
};

#endif
