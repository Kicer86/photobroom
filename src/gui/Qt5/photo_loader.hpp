
#ifndef GUI_PHOTO_LOADER
#define GUI_PHOTO_LOADER

#include "iphoto_loader.hpp"

class PhotoLoader: public IPhotoLoader::INotifier
{
    public:
        PhotoLoader();
        virtual ~PhotoLoader();

        virtual void thumbnailReady(const QString &) override;
};

#endif
