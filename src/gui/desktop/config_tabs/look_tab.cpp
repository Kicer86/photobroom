
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



LookTabControler::LookTabControler(): m_configuration(nullptr), m_tabWidget(nullptr)
{
}


LookTabControler::~LookTabControler()
{
}


void LookTabControler::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}


QString LookTabControler::tabId() const
{
    return "LookTab";
}


QString LookTabControler::tabName() const
{
    return tr("Look");
}


QWidget* LookTabControler::constructTab()
{
    m_tabWidget = new LookTab;

    return m_tabWidget;
}


void LookTabControler::applyConfiguration()
{
}


void LookTabControler::rejectConfiguration()
{
}
