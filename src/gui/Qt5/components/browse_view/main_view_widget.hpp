
#ifndef MAIN_VIEW_WIDGET_HPP
#define MAIN_VIEW_WIDGET_HPP

#include <QWidget>


namespace Database
{
    struct IDatabase;
}

struct ITaskExecutor;
struct IConfiguration;
class DBDataModel;
class ImagesTreeView;

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
        void set(IConfiguration *);

    private:
        DBDataModel* m_imagesModel;
        ImagesTreeView*  m_imagesView;

    private slots:
        void refresh();
        void basicFiltersUpdated(int);
};

#endif // MAIN_VIEW_WIDGET_HPP
