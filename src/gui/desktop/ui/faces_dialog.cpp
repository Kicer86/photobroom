#include "faces_dialog.hpp"
#include "ui_faces_dialog.h"

FacesDialog::FacesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FacesDialog)
{
    ui->setupUi(this);
}

FacesDialog::~FacesDialog()
{
    delete ui;
}
