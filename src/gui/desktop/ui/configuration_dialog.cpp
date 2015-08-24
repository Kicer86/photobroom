#include "configuration_dialog.hpp"
#include "ui_configuration_dialog.h"

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);
}


ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}


void ConfigurationDialog::on_buttonBox_accepted()
{
    accept();
}


void ConfigurationDialog::on_buttonBox_rejected()
{
    reject();
}
