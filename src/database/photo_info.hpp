
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>
#include <cstdint>
#include <string>

#include <QString>

#include "iphoto_info.hpp"

#include "database_export.h"
#include <OpenLibrary/putils/ts_resource.hpp>

class QPixmap;

struct Sha256Assigner;

namespace Photo
{
    struct Data;
}

class DATABASE_EXPORT PhotoInfo final: public IPhotoInfo, ol::ThreadSafeResource<Tag::TagsList>::INotify
{
    public:
        PhotoInfo(const QString &path);                      //load all data from provided path
        PhotoInfo(const Photo::Data &);
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
        bool isExifDataLoaded() const override;              // returns true is tags were loaded

        //observers
        void registerObserver(IObserver *) override;
        void unregisterObserver(IObserver *) override;

        //data initializing
        void setSha256(const Photo::Sha256sum &) override;

        //set data
        void setTags(const Tag::TagsList &) override;
        void setTag(const TagNameInfo &, const TagValue &) override;

        //flags
        void markFlag(Photo::FlagsE, int) override;
        int getFlag(Photo::FlagsE) const override;

        // other
        void invalidate() override;
        bool isValid() override;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;

        void updated();

        //INotify:
        void unlocked() override;
};

#endif
