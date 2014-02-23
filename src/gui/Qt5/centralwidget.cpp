
#include "centralwidget.hpp"

#include "widgets/browse_view/main_view_widget.hpp"
#include "widgets/photos_staging/photos_staging_area.hpp"

CentralWidget::CentralWidget(QWidget *p): QTabWidget(p)
{
    addTab(new MainViewWidget(this), tr("Photos"));
    addTab(new PhotosStagingArea(this), tr("Add photos"));
}
