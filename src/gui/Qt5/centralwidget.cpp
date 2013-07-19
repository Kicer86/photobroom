
#include "centralwidget.hpp"

#include "main_view_widget.hpp"

CentralWidget::CentralWidget(QWidget *parent): QTabWidget(parent)
{
    addTab(new MainViewWidget, "Photos");
}
