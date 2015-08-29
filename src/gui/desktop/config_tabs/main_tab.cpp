
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


MainTabControler::MainTabControler(): m_tabWidget(nullptr)
{

}


MainTabControler::~MainTabControler()
{

}


QString MainTabControler::tabId() const
{
    return "MainTab";
}


QString MainTabControler::tabName() const
{
    return tr("Main");
}

QWidget* MainTabControler::constructTab()
{
    m_tabWidget = new MainTab;

    return m_tabWidget;
}


void MainTabControler::applyConfiguration()
{

}


void MainTabControler::rejectConfiguration()
{

}
