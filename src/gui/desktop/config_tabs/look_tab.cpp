
#include "look_tab.hpp"

#include "ui_look_tab.h"

LookTab::LookTab(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::LookTab)
{
    ui->setupUi(this);
}

LookTab::~LookTab()
{
    delete ui;
}
