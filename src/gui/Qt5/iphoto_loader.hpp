
#ifndef GUI_IPHOTO_LOADER_HPP
#define GUI_IPHOTO_LOADER_HPP

#include <QPixmap>

class QString;

struct IPhotoLoader
{
    struct INotifier
    {
        virtual ~INotifier() {};
        virtual void thumbnailReady(const QString &path) = 0;
    };

    virtual ~IPhotoLoader() {}

    virtual void setNotifier(INotifier *) = 0;
    virtual void generateThumbnail(const QString& path) = 0;
    virtual QPixmap getThumbnailFor(const QString& path) = 0;
};

#endif
