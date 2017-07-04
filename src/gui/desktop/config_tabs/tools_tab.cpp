
#include "tools_tab.hpp"
#include "ui_tools_tab.h"

#include <QFileDialog>

#include <configuration/iconfiguration.hpp>

#include "config_keys.hpp"


ToolsTab::ToolsTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolsTab)
{
    ui->setupUi(this);

    auto chooseExecutable = [this]()
    {
        return QFileDialog::getOpenFileName(this, tr("Select executable"),
                                            "",
                                            tr("Executables (*.*)"));
    };

    ui->aisPath->setButton(tr("Browse"));
    ui->aisPath->setDialogCallback(chooseExecutable);
    ui->convertPath->setButton(tr("Browse"));
    ui->convertPath->setDialogCallback(chooseExecutable);
}


ToolsTab::~ToolsTab()
{
    delete ui;
}

QtExtChooseFile* ToolsTab::aisPath() const
{
    return ui->aisPath;
}


QtExtChooseFile* ToolsTab::convertPath() const
{
    return ui->convertPath;
}


ToolsTabControler::ToolsTabControler()
{
}


ToolsTabControler::~ToolsTabControler()
{
}


int ToolsTabControler::tabId() const
{
    return 20;
}


QString ToolsTabControler::tabName() const
{
    return tr("External tools");
}


QWidget* ToolsTabControler::constructTab()
{
    ToolsTab* tab = new ToolsTab;
    setTabWidget(tab);

    IConfiguration* config = configuration();

    const QString aisPath = config->getEntry(ExternalToolsConfigKeys::aisPath).toString();
    const QString convertPath = config->getEntry(ExternalToolsConfigKeys::convertPath).toString();

    tab->aisPath()->setValue(aisPath);
    tab->convertPath()->setValue(convertPath);

    return tab;
}


void ToolsTabControler::applyConfiguration()
{
    ToolsTab* tab = tabWidget();

    const QString aisPath = tab->aisPath()->value();
    const QString convertPath = tab->convertPath()->value();

    IConfiguration* config = configuration();

    config->setEntry(ExternalToolsConfigKeys::aisPath, aisPath);
    config->setEntry(ExternalToolsConfigKeys::convertPath, convertPath);
}


void ToolsTabControler::rejectConfiguration()
{
}
