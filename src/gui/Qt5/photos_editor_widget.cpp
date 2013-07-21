
#include "photos_editor_widget.hpp"


struct PhotosEditorWidget::GuiData
{
    GuiData() {}
    ~GuiData() {}
};

PhotosEditorWidget::PhotosEditorWidget(QWidget *parent): QWidget(parent), m_gui(new GuiData)
{
}


PhotosEditorWidget::~PhotosEditorWidget()
{

}


void PhotosEditorWidget::addPhoto(const std::string &photo)
{

}
