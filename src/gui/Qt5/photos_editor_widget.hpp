
#ifndef PHOTOS_EDITOR_WIDGET_HPP
#define PHOTOS_EDITOR_WIDGET_HPP

#include <memory>

#include <QWidget>

class PhotosEditorWidget : public QWidget
{
    public:
        explicit PhotosEditorWidget(QWidget *parent = 0);
        virtual ~PhotosEditorWidget();

        template<class T>
        void addPhotos(const T &collection)
        {
            for(auto &photo: collection)
            {
                addPhoto(photo);
            }
        }

        void addPhoto(const std::string &);

    private:
        struct GuiData;
        std::unique_ptr<GuiData> m_gui;
};

#endif // PHOTOS_EDITOR_WIDGET_HPP
