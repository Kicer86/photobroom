
#include "project_picker.hpp"
#include "ui_project_picker.h"

ProjectPicker::ProjectPicker(QWidget *_parent) :
    QDialog(_parent),
    ui(new Ui::ProjectPicker),
    m_choosenProjectName()
{
    ui->setupUi(this);
}


ProjectPicker::~ProjectPicker()
{
    delete ui;
}


QString ProjectPicker::choosenProjectName() const
{
    return m_choosenProjectName;
}


void ProjectPicker::on_openButton_clicked()
{
    done(QDialog::Accepted);
}


void ProjectPicker::on_newButton_clicked()
{
    
}


void ProjectPicker::on_deleteButton_clicked()
{

}
