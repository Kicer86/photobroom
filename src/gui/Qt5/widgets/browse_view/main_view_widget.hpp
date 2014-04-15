
#ifndef MAIN_VIEW_WIDGET_HPP
#define MAIN_VIEW_WIDGET_HPP

#include <QWidget>

class QAbstractItemView;
class QAbstractItemModel;

class MainViewWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit MainViewWidget(QWidget *parent = 0);
        virtual ~MainViewWidget();

        MainViewWidget(const MainViewWidget &) = delete;
        MainViewWidget& operator=(const MainViewWidget &) = delete;

    private:
        QAbstractItemModel* m_imagesModel;
        QAbstractItemView*  m_imagesView;

    private slots:
        void refresh();
};

#endif // MAIN_VIEW_WIDGET_HPP
