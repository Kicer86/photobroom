
#ifndef MAIN_VIEW_WIDGET_HPP
#define MAIN_VIEW_WIDGET_HPP

#include <QWidget>

class ImagesView;
class ImagesModel;

class MainViewWidget : public QWidget
{
    public:
        explicit MainViewWidget(QWidget *parent = 0);
        virtual ~MainViewWidget();

    private:
        ImagesModel* m_imagesModel;
        ImagesView*  m_imagesView;
};

#endif // MAIN_VIEW_WIDGET_HPP
