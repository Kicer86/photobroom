
#ifndef MAIN_VIEW_WIDGET_HPP
#define MAIN_VIEW_WIDGET_HPP

#include <QWidget>

class QAbstractItemView;
class QAbstractItemModel;

class MainViewWidget : public QWidget
{
    public:
        explicit MainViewWidget(QWidget *parent = 0);
        virtual ~MainViewWidget();

        MainViewWidget(const MainViewWidget &) = delete;
        MainViewWidget& operator=(const MainViewWidget &) = delete;

    private:
        QAbstractItemModel* m_imagesModel;
        QAbstractItemView*  m_imagesView;
};

#endif // MAIN_VIEW_WIDGET_HPP
