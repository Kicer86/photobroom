
#include "preparing_photos_widget.hpp"
#include "private/preparing_photos_widget_private.hpp"

#include "photos_editor_widget.hpp"


PreparingPhotosWidget::PreparingPhotosWidget(QWidget *parent): QWidget(parent), m_gui(new GuiData(this))
{

}


PreparingPhotosWidget::~PreparingPhotosWidget()
{
}

//#include "moc_preparing_photos_widget.h"
