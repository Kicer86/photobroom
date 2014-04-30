
#ifndef GUI_PHOTO_INFO_HPP
#define GUI_PHOTO_INFO_HPP

#include <QObject>

#include "core/aphoto_info.hpp"

class QPixmap;
class QImage;

struct IStreamFactory;
struct ThumbnailGenerator;

class PhotoInfo: public APhotoInfo
{
    public:
        PhotoInfo(const std::string& path);

        virtual ~PhotoInfo();

        const QPixmap& getThumbnail() const;

        PhotoInfo(const PhotoInfo &) = delete;
        PhotoInfo& operator=(const PhotoInfo &) = delete;

    private:
        friend struct ThumbnailGenerator;
        QPixmap* m_thumbnail;

        void load();
        void thumbnailReady(const QPixmap &);
};


#endif
