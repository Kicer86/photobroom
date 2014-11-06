
#include "project_picker.hpp"
#include "ui_project_picker.h"

ProjectPicker::ProjectPicker(QWidget *_parent) :
    QWidget(_parent),
    ui(new Ui::ProjectPicker)
{
    ui->setupUi(this);
}


ProjectPicker::~ProjectPicker()
{
    delete ui;
}


void ProjectPicker::on_openButton_clicked()
{

}


void ProjectPicker::on_newButton_clicked()
{

}

