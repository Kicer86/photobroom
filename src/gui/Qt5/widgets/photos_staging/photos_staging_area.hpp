
#ifndef PREPARING_PHOTOS_WIDGET_H
#define PREPARING_PHOTOS_WIDGET_H

#include <QWidget>

#include <core/photo_info.hpp>

namespace Database
{
    struct IFrontend;
}

class TagEditorWidget;
class PhotosViewWidget;
class QItemSelection;

class PhotosStagingArea: public QWidget
{
        Q_OBJECT

    public:
        explicit PhotosStagingArea(Database::IFrontend *, QWidget * = 0);
        virtual ~PhotosStagingArea();

        PhotosStagingArea(const PhotosStagingArea &) = delete;
        void operator=(const PhotosStagingArea &) = delete;

    private:
        PhotosViewWidget *m_editor;
        TagEditorWidget  *m_tagEditor;
        Database::IFrontend* m_frontend;

    private slots:
        void pathToAnalyze(QString);
        void viewSelectionChanged(const std::vector< PhotoInfo::Ptr >&);
        void savePhotos();

};

#endif // PREPARING_PHOTOS_WIDGET_H
