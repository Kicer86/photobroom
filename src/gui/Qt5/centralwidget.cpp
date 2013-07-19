
#include "centralwidget.hpp"

#include "main_view_widget.hpp"
#include "preparing_photos_widget.hpp"

CentralWidget::CentralWidget(QWidget *parent): QTabWidget(parent)
{
    addTab(new MainViewWidget, tr("Photos"));
    addTab(new PreparingPhotosWidget, tr("Add photos"));
}
