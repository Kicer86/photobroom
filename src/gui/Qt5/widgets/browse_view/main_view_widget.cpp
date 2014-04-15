
#include "main_view_widget.hpp"

#include <QVBoxLayout>
#include <QTreeView>
#include <QPushButton>

#include <database/databasebuilder.hpp>

#include "model_view/images_tree_view.hpp"
#include "model_view/images_model.hpp"
#include "model_view/db_data_model.hpp"

MainViewWidget::MainViewWidget(QWidget *p): QWidget(p), m_imagesModel(nullptr), m_imagesView(nullptr)
{
    DBDataModel* dbModel = new DBDataModel(this);
    dbModel->setBackend(Database::Builder::instance()->getBackend());

    m_imagesModel = dbModel;
    m_imagesView = new ImagesTreeView(this);

    m_imagesView->setModel(m_imagesModel);

    QVBoxLayout* main_layout = new QVBoxLayout(this);
    main_layout->addWidget(m_imagesView);

    QPushButton* button = new QPushButton("Refresh", this);
    connect(button, SIGNAL(pressed()), this, SLOT(refresh()));
    main_layout->addWidget(button);
}


MainViewWidget::~MainViewWidget()
{

}


void MainViewWidget::refresh()
{
    m_imagesView->setModel(nullptr);
    m_imagesView->setModel(m_imagesModel);
}

