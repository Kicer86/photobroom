
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>
#include <cstdint>

class QString;
class QPixmap;

struct ITagData;

struct RawPhotoData
{
    uint8_t* data;
    size_t   size;
};

class APhotoInfo
{
    public:
        typedef std::shared_ptr<APhotoInfo> Ptr;

        APhotoInfo(const std::string &path);
        APhotoInfo(const APhotoInfo &);
        virtual ~APhotoInfo();

        const std::string& getPath() const;

        std::shared_ptr<ITagData> getTags() const;

        //photo data
        virtual RawPhotoData rawPhotoData() = 0;
        virtual RawPhotoData rawThumbnailData() = 0;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;
};

#endif
