
#ifndef GUI_PHOTO_LOADER
#define GUI_PHOTO_LOADER

#include <memory>

#include <QObject>

#include "iphoto_loader.hpp"

class PhotoLoader: public IPhotoLoader
{
    public:
        struct Data;
        
        PhotoLoader();
        virtual ~PhotoLoader();

        void generateThumbnail(const QString &, IPhotoLoader::INotifier *) override;
        QPixmap getThumbnailFor(const QString &) override;
    private:
        std::unique_ptr<Data> m_data;
};

#endif
