
#ifndef __CENTRAL_WIDGET_HPP__
#define __CENTRAL_WIDGET_HPP__

#include <QTabWidget>

class ITaskExecutor;

namespace Database
{
    struct IDatabase;
}

class MainViewWidget;
class PhotosStagingArea;

class CentralWidget: public QTabWidget
{
    public:
        CentralWidget(QWidget *);
        CentralWidget(const CentralWidget &) = delete;

        CentralWidget& operator=(const CentralWidget &) = delete;

        void setDatabase(Database::IDatabase *);
        void set(ITaskExecutor *);

    private:
        MainViewWidget* m_viewWidget;
        PhotosStagingArea* m_stagingArea;
};


#endif
