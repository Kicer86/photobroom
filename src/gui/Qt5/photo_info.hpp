
#ifndef GUI_PHOTO_INFO_HPP
#define GUI_PHOTO_INFO_HPP

#include "core/photo_info.hpp"


template<typename T>
class PhotoInfo: public APhotoInfo
{
    public:

        struct IManipulator
        {
            virtual ~IManipulator() {}

            virtual void set(T* photo, T* thumbnail) = 0;
            virtual void load(const std::string &) = 0;

            virtual RawPhotoData rawPhoto() = 0;
            virtual RawPhotoData rawThumbnail() = 0;
        };

        PhotoInfo(const std::string& path, IManipulator* manipulator):
            APhotoInfo(path),
            m_photo(new T),
            m_thumbnail(new T),
            m_manipulator(manipulator)
        {
            m_manipulator->set(m_photo.get(), m_thumbnail.get());
            m_manipulator->load(path);
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

        const T& getPhoto() const
        {
            return *m_photo.get();
        }

        const T& getThumbnail() const
        {
            return *m_thumbnail.get();
        }

        PhotoInfo(const PhotoInfo<T> &) = delete;
        PhotoInfo<T>& operator=(const PhotoInfo<T> &) = delete;

    private:
        std::unique_ptr<T> m_photo;
        std::unique_ptr<T> m_thumbnail;
        IManipulator* m_manipulator;
};


#endif
