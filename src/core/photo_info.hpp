
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>

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

#endif
