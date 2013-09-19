
#ifndef PHOTO_INFO_HPP 
#define PHOTO_INFO_HPP

#include <memory>

class QString;
class QPixmap;

class PhotoInfo
{
    public:
        typedef std::shared_ptr<PhotoInfo> PhotoInfoPtr;
        
        PhotoInfo(const QString &path);
        PhotoInfo(const PhotoInfo &);
        virtual ~PhotoInfo();
        
        const QString& getPath() const;
        const QPixmap& getPixmap() const;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;
};

#endif
