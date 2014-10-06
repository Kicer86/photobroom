#include "photos_adding_wizard.hpp"
#include "ui_photos_adding_wizard.h"

PhotosAddingWizard::PhotosAddingWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::PhotosAddingWizard)
{
    ui->setupUi(this);
}

PhotosAddingWizard::~PhotosAddingWizard()
{
    delete ui;
}
