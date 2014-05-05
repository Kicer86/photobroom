
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>
#include <cstdint>
#include <string>

#include "core_export.h"

class QPixmap;

struct ITagData;
struct APhotoInfoInitData;
struct HashAssigner;

class CORE_EXPORT PhotoInfo final
{
    public:
        struct IObserver
        {
            IObserver() {};
            virtual void photoUpdated() = 0;
        };

        typedef std::shared_ptr<PhotoInfo> Ptr;
        typedef std::string Hash;                // Hash is hash of photo's content (pixels) not whole file itself.

        PhotoInfo(const std::string &path);      //load all data from provided path
        PhotoInfo(const APhotoInfoInitData &);   //load all data from provided struct
        PhotoInfo(const PhotoInfo &) = delete;
        virtual ~PhotoInfo();

        const std::string& getPath() const;
        std::shared_ptr<ITagData> getTags() const;
        const QPixmap& getThumbnail() const;     // a temporary thumbnail may be returned when final one is not yet generated.
        const Hash& getHash() const;             // Do not call until isHashLoaded()

        bool isLoaded() const;                   // returns true if hash is not null, and thumbnail is not temporary one.
        bool isHashLoaded() const;               // returns true if hash is not null
        bool isThumbnailLoaded() const;          // returns true if thumbnail is loaded

        void registerObserver(IObserver *);
        void unregisterObserver(IObserver *);
        void setHash(const Hash &);
        void setThumbnail(const QPixmap &);

    protected:
        void updated();

    private:
        struct Data;
        std::unique_ptr<Data> m_data;

};


struct CORE_EXPORT APhotoInfoInitData
{
    APhotoInfoInitData();

    std::string path;
    std::shared_ptr<ITagData> tags;
    PhotoInfo::Hash hash;
};

#endif
