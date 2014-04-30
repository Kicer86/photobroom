
#ifndef PHOTOS_EDITOR_WIDGET_HPP
#define PHOTOS_EDITOR_WIDGET_HPP

#include <QWidget>

#include <core/aphoto_info.hpp>
#include <core/time_guardian.hpp>

class QItemSelectionModel;
class QItemSelection;

class DBDataModel;
class ImagesTreeView;


class PhotosViewWidget: public QWidget
{
        Q_OBJECT

    public:
        explicit PhotosViewWidget(QWidget *parent = 0);
        PhotosViewWidget(const PhotosViewWidget &) = delete;
        virtual ~PhotosViewWidget();

        void operator=(const PhotosViewWidget &) = delete;

        template<class T>
        void addPhotos(const T &collection)
        {
            TIME_GUARDIAN("Photos storage", 100, "Too long!");

            for(auto &photo: collection)
            {
                addPhoto(photo);
            }
        }

        void addPhoto(const std::string &);
        std::vector<APhotoInfo::Ptr> getPhotos() const;

    private:
        DBDataModel*    m_photosModel;
        ImagesTreeView* m_photosView;

    private slots:
        void selectionChanged();

    signals:
        void selectionChanged(const std::vector<APhotoInfo::Ptr> &);
};

#endif // PHOTOS_EDITOR_WIDGET_HPP
