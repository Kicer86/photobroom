
#ifndef GUI_PHOTO_INFO_HPP
#define GUI_PHOTO_INFO_HPP

#include <QObject>

#include "core/aphoto_info.hpp"
#include "thread_multiplexer.hpp"

class QPixmap;
class QImage;

struct IStreamFactory;
struct ThumbnailGenerator;

class PhotoInfo: public APhotoInfo
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
        friend struct ThumbnailGenerator;
        QPixmap* m_thumbnail;
        QImage*  m_thumbnailRaw;
        ThreadMultiplexer m_multpilexer;

        void load();
        void thumbnailReady(const QPixmap &);


};


#endif
