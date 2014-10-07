
#include "photos_view.hpp"

#include <core/base_tags.hpp>

#include "model_view/db_data_model.hpp"
#include "ui_photos_view.h"

PhotosView::PhotosView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PhotosView),
    m_imagesModel(nullptr)
{
    ui->setupUi(this);

    ui->sortingCombo->addItem("Date and time");
    ui->sortingCombo->addItem("People");
}

PhotosView::~PhotosView()
{
    delete ui;
}


void PhotosView::set(IConfiguration* configuration)
{
    ui->photoView->set(configuration);
}


void PhotosView::setModel(DBDataModel* model)
{
    m_imagesModel = model;
}


void PhotosView::on_sortingCombo_currentIndexChanged(int index)
{
    if (index == 0)
    {
        Hierarchy hierarchy;
        hierarchy.levels = { { BaseTags::get(BaseTagsList::Date), Hierarchy::Level::Order::ascending }  };

        m_imagesModel->setHierarchy(hierarchy);
    }
    else
    {
        Hierarchy hierarchy;
        hierarchy.levels = { { BaseTags::get(BaseTagsList::People), Hierarchy::Level::Order::ascending }  };

        m_imagesModel->setHierarchy(hierarchy);
    }
}
