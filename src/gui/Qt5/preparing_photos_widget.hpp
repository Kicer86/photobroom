
#ifndef PREPARING_PHOTOS_WIDGET_H
#define PREPARING_PHOTOS_WIDGET_H

#include <QWidget>

class TagEditorWidget;
class PhotosViewWidget;
class QItemSelection;

class PreparingPhotosWidget: public QWidget
{
        Q_OBJECT

    public:
        explicit PreparingPhotosWidget(QWidget *parent = 0);
        virtual ~PreparingPhotosWidget();

        PreparingPhotosWidget(const PreparingPhotosWidget &) = delete;
        void operator=(const PreparingPhotosWidget &) = delete;

    private:
        PhotosViewWidget *m_editor;
        TagEditorWidget  *m_tagEditor;

    private slots:
        void pathToAnalyze(QString);
        void viewSelectionChanged(const QItemSelection &);

};

#endif // PREPARING_PHOTOS_WIDGET_H
