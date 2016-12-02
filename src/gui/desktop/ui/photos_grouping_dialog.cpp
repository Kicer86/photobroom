
#include "photos_grouping_dialog.hpp"

#include "ui_photos_grouping_dialog.h"


PhotosGroupingDialog::PhotosGroupingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PhotosGroupingDialog)
{
    ui->setupUi(this);
}


PhotosGroupingDialog::~PhotosGroupingDialog()
{
    delete ui;
}
