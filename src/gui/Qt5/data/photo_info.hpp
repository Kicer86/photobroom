
#ifndef PHOTO_INFO_HPP 
#define PHOTO_INFO_HPP

#include <memory>

class QString;
class QPixmap;

struct ITagData;

class PhotoInfo
{
    public:
        typedef std::shared_ptr<PhotoInfo> Ptr;
        
        PhotoInfo(const QString &path);
        PhotoInfo(const PhotoInfo &);
        virtual ~PhotoInfo();
        
        const QString& getPath() const;
        const QPixmap& getPixmap() const;
        const ITagData& getTags() const;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;
};

#endif
