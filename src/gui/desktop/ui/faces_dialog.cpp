
#include "faces_dialog.hpp"

#include <core/icore_factory_accessor.hpp>
#include <core/ipython_thread.hpp>

#include "ui_faces_dialog.h"


FacesDialog::FacesDialog(ICoreFactoryAccessor* coreAccessor, QWidget *parent):
    QDialog(parent),
    ui(new Ui::FacesDialog),
    m_pythonThread(coreAccessor->getPythonThread())
{
    ui->setupUi(this);
}


FacesDialog::~FacesDialog()
{
    delete ui;
}


void FacesDialog::load(const QString& photo)
{
    QPixmap pixmap(photo);
    ui->imageView->setPixmap(pixmap);
}
