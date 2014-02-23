
#include "main_view_widget.hpp"

#include <QVBoxLayout>

#include "model_view/images_view.hpp"
#include "model_view/images_model.hpp"

MainViewWidget::MainViewWidget(QWidget *p): QWidget(p), m_imagesModel(nullptr), m_imagesView(nullptr)
{
    m_imagesModel = new ImagesModel(this);
    m_imagesView = new ImagesView(this);

    QVBoxLayout* main_layout = new QVBoxLayout(this);
    main_layout->addWidget(m_imagesView);
}


MainViewWidget::~MainViewWidget()
{

}

