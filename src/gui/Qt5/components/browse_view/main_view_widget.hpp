
#ifndef MAIN_VIEW_WIDGET_HPP
#define MAIN_VIEW_WIDGET_HPP

#include <QWidget>


class QAbstractItemView;

namespace Database
{
    struct IDatabase;
}
struct ITaskExecutor;
class DBDataModel;

class MainViewWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit MainViewWidget(QWidget *parent = 0);
        virtual ~MainViewWidget();

        MainViewWidget(const MainViewWidget &) = delete;
        MainViewWidget& operator=(const MainViewWidget &) = delete;

        void setDatabase(Database::IDatabase *);
        void set(ITaskExecutor *);

    private:
        DBDataModel* m_imagesModel;
        QAbstractItemView*  m_imagesView;

    private slots:
        void refresh();
        void basicFiltersUpdated(int);
};

#endif // MAIN_VIEW_WIDGET_HPP
