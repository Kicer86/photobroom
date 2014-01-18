
#ifndef GUI_PHOTO_INFO_HPP
#define GUI_PHOTO_INFO_HPP

#include <QObject>

#include "core/aphoto_info.hpp"
#include "iphoto_loader.hpp"
#include "thread_multiplexer.hpp"

class QPixmap;
class QImage;

struct IStreamFactory;

class PhotoInfo: public APhotoInfo, IPhotoLoader::INotifier
{
    public:
        PhotoInfo(const std::string& path, ThreadMultiplexer::IGetter *);   //getter will be informed when a certain thumbnail is ready

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
        ThreadMultiplexer m_multpilexer;

        void load();
        virtual void thumbnailReady(const QString &) override;


};


#endif
