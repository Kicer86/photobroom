
#include "photos_grouping_dialog.hpp"

#include "ui_photos_grouping_dialog.h"
#include "ui_utils/photos_item_delegate.hpp"


PhotosGroupingDialog::PhotosGroupingDialog(IThumbnailAcquisitor* th_acq, IConfiguration* config, QWidget *parent):
    QDialog(parent),
    m_model(),
    ui(new Ui::PhotosGroupingDialog)
{
    ui->setupUi(this);
    ui->photosView->setModel(&m_model);

    PhotosItemDelegate* delegate = new PhotosItemDelegate(ui->photosView);
    delegate->set(th_acq);
    delegate->set(config);

    ui->photosView->setItemDelegate(delegate);
}


PhotosGroupingDialog::~PhotosGroupingDialog()
{
    delete ui;
}


void PhotosGroupingDialog::set(const std::vector<IPhotoInfo::Ptr>& photos)
{
    m_model.set(photos);
}
