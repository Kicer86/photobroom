
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


void ConfigurationDialog::addTab(const QString& name, QWidget* widget)
{
    ui->tabWidget->addTab(widget, name);
}


void ConfigurationDialog::on_buttonBox_accepted()
{
    accept();
}


void ConfigurationDialog::on_buttonBox_rejected()
{
    reject();
}

void ConfigurationDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    auto role = ui->buttonBox->buttonRole(button);

    if (role == QDialogButtonBox::ApplyRole || role == QDialogButtonBox::AcceptRole)
        emit saveData();
}
