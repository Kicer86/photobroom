
#ifndef PHOTOS_EDITOR_WIDGET_HPP
#define PHOTOS_EDITOR_WIDGET_HPP

#include <QWidget>

class QItemSelectionModel;
class QItemSelection;


struct GuiDataSlots: public QObject
{
        Q_OBJECT

    public:
        GuiDataSlots(QObject *);
        virtual ~GuiDataSlots();

    protected slots:
        virtual void selectionChanged(const QItemSelection &) = 0;
};


class PhotosEditorWidget: public QWidget
{
        Q_OBJECT
        
    public:
        explicit PhotosEditorWidget(QWidget *parent = 0);
        PhotosEditorWidget(const PhotosEditorWidget &) = delete;
        virtual ~PhotosEditorWidget();

        void operator=(const PhotosEditorWidget &) = delete;
        
        template<class T>
        void addPhotos(const T &collection)
        {
            for(auto &photo: collection)
            {
                addPhoto(photo);
            }
        }

        void addPhoto(const std::string &);

    protected:

    private:
        struct GuiData;
        GuiData *m_gui;
        
    signals:
        void selectionChanged();
};

#endif // PHOTOS_EDITOR_WIDGET_HPP
