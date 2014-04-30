
#ifndef PREPARING_PHOTOS_WIDGET_H
#define PREPARING_PHOTOS_WIDGET_H

#include <QWidget>

#include "core/aphoto_info.hpp"

class TagEditorWidget;
class PhotosViewWidget;
class QItemSelection;

class PhotosStagingArea: public QWidget
{
        Q_OBJECT

    public:
        explicit PhotosStagingArea(QWidget *parent = 0);
        virtual ~PhotosStagingArea();

        PhotosStagingArea(const PhotosStagingArea &) = delete;
        void operator=(const PhotosStagingArea &) = delete;

    private:
        PhotosViewWidget *m_editor;
        TagEditorWidget  *m_tagEditor;

    private slots:
        void pathToAnalyze(QString);
        void viewSelectionChanged(const std::vector< PhotoInfo::Ptr >&);
        void savePhotos();

};

#endif // PREPARING_PHOTOS_WIDGET_H
