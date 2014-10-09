#include "staged_photos_view.hpp"
#include "ui_staged_photos_view.h"

StagedPhotosView::StagedPhotosView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StagedPhotosView)
{
    ui->setupUi(this);
}

StagedPhotosView::~StagedPhotosView()
{
    delete ui;
}
