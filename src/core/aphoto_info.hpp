
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>
#include <cstdint>
#include <string>

#include "core_export.h"

class QString;
class QPixmap;

struct ITagData;

struct CORE_EXPORT RawPhotoData
{
    uint8_t* data;
    size_t   size;

    RawPhotoData();
    ~RawPhotoData();

    RawPhotoData(const RawPhotoData &) = delete;
    RawPhotoData(RawPhotoData&& other);

    RawPhotoData& operator=(const RawPhotoData &) = delete;
};

struct APhotoInfoInitData
{
};

struct HashAssigner;

class CORE_EXPORT APhotoInfo
{
    public:
        typedef std::shared_ptr<APhotoInfo> Ptr;
        typedef std::string Hash;

        APhotoInfo(const std::string &path);
        APhotoInfo(const APhotoInfoInitData &);
        APhotoInfo(const APhotoInfo &);
        virtual ~APhotoInfo();

        const std::string& getPath() const;

        std::shared_ptr<ITagData> getTags() const;

        //photo data
        virtual RawPhotoData rawPhotoData() = 0;
        virtual RawPhotoData rawThumbnailData() = 0;   // a temporary thumbnail may be returned when final one is not yet generated

        // Function may return empty hash, when it is not yet calculated.
        // The returned value is hash of photo's content (pixels) not whole file itself.
        const Hash& getHash() const;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;

        friend struct HashAssigner;
        void setHash(const Hash &);
};

#endif
