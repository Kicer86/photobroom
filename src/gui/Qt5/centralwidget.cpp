
#include "centralwidget.hpp"

#include "widgets/browse_view/main_view_widget.hpp"
#include "widgets/photos_staging/photos_staging_area.hpp"

CentralWidget::CentralWidget(QWidget *p): QTabWidget(p)
{
    MainViewWidget* viewWidget = new MainViewWidget(this);
    addTab(viewWidget, tr("Photos"));
    addTab(new PhotosStagingArea(viewWidget->getFronted(), this), tr("Add photos"));
}
