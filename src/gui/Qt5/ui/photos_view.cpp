
#include "photos_view.hpp"

#include <core/base_tags.hpp>

#include "model_view/db_data_model.hpp"
#include "ui_photos_view.h"

PhotosView::PhotosView(QWidget *_parent) :
    QWidget(_parent),
    ui(new Ui::PhotosView),
    m_imagesModel(nullptr)
{
    ui->setupUi(this);
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
    if (m_imagesModel == nullptr)
    {
        m_imagesModel = model;
        ui->photoView->setModel(model);

        ui->sortingCombo->addItem(tr("Date and time"));
        ui->sortingCombo->addItem(tr("People"));

        //pass models to tags editor
        QItemSelectionModel* selectionModel = ui->photoView->selectionModel();
        ui->tagEditor->set(selectionModel);
        ui->tagEditor->set(model);
    }
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
