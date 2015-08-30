
#include "main_tab.hpp"

#include <QCheckBox>

#include <configuration/iconfiguration.hpp>

#include "config_keys.hpp"
#include "ui_main_tab.h"


MainTab::MainTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainTab)
{
    ui->setupUi(this);

    const QStringList checks = { tr("Every day"), tr("Every 3 days"), tr("Every Week") };
    ui->freqComboBox->addItems(checks);
}


MainTab::~MainTab()
{
    delete ui;
}


QCheckBox* MainTab::updateCheckBox()
{
    return ui->updatesCheckBox;
}


QComboBox* MainTab::updateFrequency()
{
    return ui->freqComboBox;
}


MainTabControler::MainTabControler(): m_configuration(nullptr), m_tabWidget(nullptr)
{

}


MainTabControler::~MainTabControler()
{

}


void MainTabControler::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}



int MainTabControler::tabId() const
{
    return 0;
}


QString MainTabControler::tabName() const
{
    return tr("Main");
}

QWidget* MainTabControler::constructTab()
{
    m_tabWidget = new MainTab;

    const auto enabled = m_configuration->getEntry(UpdateConfigKeys::updateEnabled);
    const auto frequency = m_configuration->getEntry(UpdateConfigKeys::updateFrequency);

    m_tabWidget->updateCheckBox()->setChecked(enabled.toBool());
    m_tabWidget->updateFrequency()->setCurrentIndex(frequency.toInt());

    return m_tabWidget;
}


void MainTabControler::applyConfiguration()
{
    const bool enabled = m_tabWidget->updateCheckBox()->checkState() == Qt::Checked;
    const int frequency = m_tabWidget->updateFrequency()->currentIndex();

    m_configuration->setEntry(UpdateConfigKeys::updateEnabled, enabled);
    m_configuration->setEntry(UpdateConfigKeys::updateFrequency, frequency);
}


void MainTabControler::rejectConfiguration()
{

}
