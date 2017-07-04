
#include "tools_tab.hpp"
#include "ui_tools_tab.h"


ToolsTab::ToolsTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolsTab)
{
    ui->setupUi(this);
}


ToolsTab::~ToolsTab()
{
    delete ui;
}


ToolsTabControler::ToolsTabControler()
{
}


ToolsTabControler::~ToolsTabControler()
{
}


void ToolsTabControler::set(IConfiguration* config)
{
    m_configuration = config;
}
