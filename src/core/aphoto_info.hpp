
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>
#include <cstdint>
#include <string>

#include "core_export.h"

class QString;
class QPixmap;

struct ITagData;

struct CORE_EXPORT __attribute__((deprecated)) RawPhotoData
{
    uint8_t* data;
    size_t   size;

    RawPhotoData();
    ~RawPhotoData();

    RawPhotoData(const RawPhotoData &) = delete;
    RawPhotoData(RawPhotoData&& other);

    RawPhotoData& operator=(const RawPhotoData &) = delete;
};

struct APhotoInfoInitData;
struct HashAssigner;


struct IPhotoInfo
{
    typedef std::shared_ptr<IPhotoInfo> Ptr;
    typedef std::string Hash;

    virtual ~IPhotoInfo() {}

    virtual const std::string& getPath() const = 0;
    virtual std::shared_ptr<ITagData> getTags() const = 0;   // read-write access to tags

    //photo data
    virtual const RawPhotoData& rawPhotoData() = 0;
    virtual const RawPhotoData& rawThumbnailData() = 0;      // a temporary thumbnail may be returned when final one is not yet generated

    virtual const QPixmap& getThumbnail() const = 0;

    // Function may return empty hash, when it is not yet calculated.
    // The returned value is hash of photo's content (pixels) not whole file itself.
    virtual const Hash& getHash() const = 0;
};

class CORE_EXPORT APhotoInfo: public IPhotoInfo
{
    public:
        APhotoInfo(const std::string &path);      //load all data from provided path
        APhotoInfo(const APhotoInfoInitData &);   //load all data from provided struct
        APhotoInfo(const APhotoInfo &);
        virtual ~APhotoInfo();

        const std::string& getPath() const;
        std::shared_ptr<ITagData> getTags() const;

        // Function may return empty hash, when it is not yet calculated.
        // The returned value is hash of photo's content (pixels) not whole file itself.
        const Hash& getHash() const;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;

        friend struct HashAssigner;
        void setHash(const Hash &);
};


struct CORE_EXPORT APhotoInfoInitData
{
    APhotoInfoInitData();

    std::string path;
    std::shared_ptr<ITagData> tags;
    APhotoInfo::Hash hash;
};

#endif
