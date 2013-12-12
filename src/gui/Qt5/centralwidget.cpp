
#include "centralwidget.hpp"

#include "main_view_widget.hpp"
#include "photos_staging_area.hpp"

CentralWidget::CentralWidget(QWidget *p): QTabWidget(p)
{
    addTab(new MainViewWidget(this), tr("Photos"));
    addTab(new PreparingPhotosWidget(this), tr("Add photos"));
}
