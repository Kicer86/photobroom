
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

        APhotoInfo(const QString &path);
        APhotoInfo(const APhotoInfo &);
        virtual ~APhotoInfo();

        const QString& getPath() const;
        const QPixmap& getPixmap() const;
        std::shared_ptr<ITagData> getTags() const;

        //photo data
        virtual RawPhotoData rawPhotoData() = 0;
        virtual RawPhotoData rawThumbnailData() = 0;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;
};

template<typename T>
class PhotoInfo: public APhotoInfo
{
    public:

        struct IManipulator
        {
            virtual ~IManipulator() {}
            virtual void load(const T &) = 0;
            virtual RawPhotoData rawPhoto() = 0;
            virtual RawPhotoData rawThumbnail() = 0;
        };

        PhotoInfo(const QString& path, IManipulator* manipulator): APhotoInfo(path), m_photoData(new T), m_manipulator(manipulator)
        {

        }

        virtual ~PhotoInfo() {}

        virtual RawPhotoData rawPhotoData() override
        {
            return m_manipulator->rawPhoto();
        }

        virtual RawPhotoData rawThumbnailData() override
        {
            return m_manipulator->rawThumbnail();
        }

        PhotoInfo(const PhotoInfo<T> &) = delete;
        PhotoInfo<T>& operator=(const PhotoInfo<T> &) = delete;

    private:
        std::unique_ptr<T> m_photoData;
        IManipulator* m_manipulator;
};

#endif
