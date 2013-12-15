
#ifndef GUI_IPHOTO_LOADER_HPP
#define GUI_IPHOTO_LOADER_HPP

struct IPhotoLoader
{
    virtual ~IPhotoLoader() {}

    virtual void setNotifier(INotifier *) = 0;
    virtual void generateThumbnail(const QString& path) = 0;
    virtual QPixmap getThumbnailFor(const QString& path) = 0;

    struct INotifier
    {
        virtual ~INotifier() {};
        void photoReady(const QString &path) = 0;
    };
};

#endif
