
#include "main_tab.hpp"

#include <QCheckBox>

#include <core/iconfiguration.hpp>

#include "config_keys.hpp"
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


QCheckBox* MainTab::updateCheckBox()
{
    return ui->updatesCheckBox;
}


MainTabController::MainTabController(): m_configuration(nullptr), m_tabWidget(nullptr)
{

}


MainTabController::~MainTabController()
{

}


void MainTabController::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}



int MainTabController::tabId() const
{
    return 0;
}


QString MainTabController::tabName() const
{
    return tr("Main");
}


QWidget* MainTabController::constructTab()
{
    m_tabWidget = new MainTab;

    const auto enabled = m_configuration->getEntry(UpdateConfigKeys::updateEnabled);

    m_tabWidget->updateCheckBox()->setChecked(enabled.toBool());

    return m_tabWidget;
}


void MainTabController::applyConfiguration()
{
    const bool enabled = m_tabWidget->updateCheckBox()->checkState() == Qt::Checked;

    m_configuration->setEntry(UpdateConfigKeys::updateEnabled, enabled);
}


void MainTabController::rejectConfiguration()
{

}
