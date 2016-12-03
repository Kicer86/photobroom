
#include "photos_grouping_dialog.hpp"

#include "ui_photos_grouping_dialog.h"


PhotosGroupingDialog::PhotosGroupingDialog(QWidget *parent):
    QDialog(parent),
    m_model(),
    ui(new Ui::PhotosGroupingDialog)
{
    ui->setupUi(this);
    ui->photosView->setModel(&m_model);
}


PhotosGroupingDialog::~PhotosGroupingDialog()
{
    delete ui;
}


void PhotosGroupingDialog::set(const std::vector<IPhotoInfo::Ptr>& photos)
{
    m_model.set(photos);
}
