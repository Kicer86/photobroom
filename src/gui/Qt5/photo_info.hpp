
#ifndef GUI_PHOTO_INFO_HPP
#define GUI_PHOTO_INFO_HPP

#include <QObject>

#include "core/photo_info.hpp"
#include "iphoto_loader.hpp"

class QPixmap;
class QImage;

class PhotoInfo: public QObject, public APhotoInfo, IPhotoLoader::INotifier
{
        Q_OBJECT

    public:
        PhotoInfo(const std::string& path);

        virtual ~PhotoInfo();

        virtual RawPhotoData rawPhotoData() override;
        virtual RawPhotoData rawThumbnailData() override;

        const QPixmap getPhoto() const;
        const QPixmap& getThumbnail() const;

        PhotoInfo(const PhotoInfo &) = delete;
        PhotoInfo& operator=(const PhotoInfo &) = delete;

    private:
        QPixmap* m_thumbnail;
        QImage*  m_thumbnailRaw;

        void load();
        void thumbnailReady(const QString &);

    signals:
        void thumbnailChanged(PhotoInfo *);
};


#endif
