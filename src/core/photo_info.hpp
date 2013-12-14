
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>
#include <cstdint>

class QString;
class QPixmap;

struct ITagData;

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
            virtual uint8_t* rawData() = 0;
        };

        PhotoInfo(const QString& path, IManipulator* manipulator): APhotoInfo(path), m_manipulator(manipulator)
        {

        }

        virtual ~PhotoInfo() {}

    private:
        T m_photoData;
        IManipulator* m_manipulator;
};

#endif
