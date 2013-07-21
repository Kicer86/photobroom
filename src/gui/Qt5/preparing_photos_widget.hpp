
#ifndef PREPARING_PHOTOS_WIDGET_H
#define PREPARING_PHOTOS_WIDGET_H


#include <QWidget>

class PreparingPhotosWidget: public QWidget
{

    public:
        explicit PreparingPhotosWidget(QWidget *parent = 0);
        virtual ~PreparingPhotosWidget();

    private:
        struct GuiData;
        GuiData *m_gui;

};

#endif // PREPARING_PHOTOS_WIDGET_H
