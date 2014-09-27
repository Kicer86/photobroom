
#include "centralwidget.hpp"

#include "components/browse_view/main_view_widget.hpp"
#include "components/photos_staging/photos_staging_area.hpp"

CentralWidget::CentralWidget(QWidget *p): QTabWidget(p), m_viewWidget(nullptr), m_stagingArea(nullptr)
{
    m_viewWidget = new MainViewWidget(this);
    //m_stagingArea = new PhotosStagingArea(this);

    addTab(m_viewWidget, tr("Photos"));
    addTab(m_stagingArea, tr("Add photos"));
}


void CentralWidget::setDatabase(Database::IDatabase* db)
{
    m_viewWidget->setDatabase(db);
    //m_stagingArea->setDatabase(db);
}


void CentralWidget::set(ITaskExecutor* taskExecutor)
{
    m_viewWidget->set(taskExecutor);
    //m_stagingArea->set(taskExecutor);
}


void CentralWidget::set(IConfiguration* configuration)
{
    m_viewWidget->set(configuration);
    //m_stagingArea->set(configuration);
}

