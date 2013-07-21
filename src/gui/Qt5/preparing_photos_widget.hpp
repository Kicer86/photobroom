
#ifndef PREPARING_PHOTOS_WIDGET_H
#define PREPARING_PHOTOS_WIDGET_H

#include <memory>

#include <QWidget>

class PreparingPhotosWidget: public QWidget
{

    public:
        explicit PreparingPhotosWidget(QWidget *parent = 0);
        virtual ~PreparingPhotosWidget();

    private:
        struct GuiData;
        std::unique_ptr<GuiData> m_gui;

};

#endif // PREPARING_PHOTOS_WIDGET_H
