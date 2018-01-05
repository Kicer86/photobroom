
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>
#include <cstdint>
#include <string>

#include <QString>

#include "iphoto_info.hpp"

#include "database_export.h"

class QPixmap;

struct IPhotoInfoStorekeeper;
struct Sha256Assigner;

namespace Photo
{
    struct Data;
}

class PhotoInfo final: public IPhotoInfo
{
    public:
        PhotoInfo(const Photo::Data &, IPhotoInfoStorekeeper *);
        PhotoInfo(const Photo::DataDelta &, IPhotoInfoStorekeeper *);
        PhotoInfo(const PhotoInfo &) = delete;
        virtual ~PhotoInfo();

        PhotoInfo& operator=(const PhotoInfo &) = delete;

        //data getting
        Photo::Data            data() const override;
        const QString          getPath() const override;
        const Tag::TagsList    getTags() const override;       // a access to tags
        const QSize            getGeometry() const override;   // get photo's geometry
        const Photo::Sha256sum getSha256() const override;     // Do not call until isSha256Loaded()
        Photo::Id              getID() const override;

        //status checking
        bool isFullyInitialized() const override;            // returns true if photo fully loaded (all items below are loaded)
        bool isSha256Loaded() const override;                // returns true if sha256 is not null
        bool isGeometryLoaded() const override;              // returns true if photo's geometry was loaded
        bool isExifDataLoaded() const override;              // returns true is exif for this photo was read

        //set data
        void setSha256(const Photo::Sha256sum &) override;
        void setGeometry(const QSize &) override;
        void setTags(const Tag::TagsList &) override;
        void setTag(const TagNameInfo &, const TagValue &) override;
        void setGroup(const GroupInfo &) override;

        //flags
        void markFlag(Photo::FlagsE, int) override;
        int getFlag(Photo::FlagsE) const override;

        // other
        void invalidate() override;
        bool isValid() override;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;
        IPhotoInfoStorekeeper* m_storekeeper;
};

#endif
