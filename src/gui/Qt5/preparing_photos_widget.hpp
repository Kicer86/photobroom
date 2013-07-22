
#ifndef PREPARING_PHOTOS_WIDGET_H
#define PREPARING_PHOTOS_WIDGET_H

#include <QWidget>

class PhotosEditorWidget;

class PreparingPhotosWidget: public QWidget
{
    public:
        explicit PreparingPhotosWidget(QWidget *parent = 0);
        virtual ~PreparingPhotosWidget();

    private:
        PhotosEditorWidget *m_editor;

    private slots:
        void pathToAnalyze(QString);

};

#endif // PREPARING_PHOTOS_WIDGET_H
