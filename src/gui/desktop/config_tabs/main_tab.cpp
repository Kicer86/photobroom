#include "main_tab.hpp"
#include "ui_main_tab.h"

MainTab::MainTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainTab)
{
    ui->setupUi(this);
}

MainTab::~MainTab()
{
    delete ui;
}
