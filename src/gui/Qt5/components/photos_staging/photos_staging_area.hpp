
#ifndef PREPARING_PHOTOS_WIDGET_H
#define PREPARING_PHOTOS_WIDGET_H

#include <QWidget>

#include <core/photo_info.hpp>

namespace Database
{
    struct IFrontend;
}

class QItemSelection;

class TagEditorWidget;
class PhotosViewWidget;
struct PhotosReceiver;

class PhotosStagingArea: public QWidget
{
        Q_OBJECT

    public:
        explicit PhotosStagingArea(QWidget * = 0);
        virtual ~PhotosStagingArea();

        PhotosStagingArea(const PhotosStagingArea &) = delete;
        void operator=(const PhotosStagingArea &) = delete;

    private:
        PhotosViewWidget *m_editor;
        TagEditorWidget  *m_tagEditor;
        std::unique_ptr<PhotosReceiver> m_photosReceiver;

    private slots:
        void pathToAnalyze(const QString &);
        void viewSelectionChanged(const std::vector< PhotoInfo::Ptr >&);
        void savePhotos();

};

#endif // PREPARING_PHOTOS_WIDGET_H
