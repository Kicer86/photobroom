
#ifndef PHOTOS_EDITOR_WIDGET_HPP
#define PHOTOS_EDITOR_WIDGET_HPP

#include <QWidget>

#include "core/aphoto_info.hpp"

class QItemSelectionModel;
class QItemSelection;


struct GuiDataSlots: public QObject
{
        Q_OBJECT

    public:
        GuiDataSlots(QObject *);
        virtual ~GuiDataSlots();

    protected slots:
        virtual void selectionChanged() = 0;
};


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
            for(auto &photo: collection)
            {
                addPhoto(photo);
            }
        }

        void addPhoto(const std::string &);

        const std::vector<APhotoInfo::Ptr>& getPhotos() const;

    protected:

    private:
        struct GuiData;
        std::unique_ptr<GuiData> m_gui;

    signals:
        void selectionChanged(const std::vector<APhotoInfo::Ptr> &);
};

#endif // PHOTOS_EDITOR_WIDGET_HPP
