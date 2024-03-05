
#include "tools_tab.hpp"
#include "ui_tools_tab.h"

#include <QFileDialog>

#include <core/iconfiguration.hpp>
#include <core/constants.hpp>


ToolsTab::ToolsTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolsTab)
{
    ui->setupUi(this);

    auto chooseExecutable = [this]()
    {
#ifdef OS_WIN
        return QFileDialog::getOpenFileName(this, tr("Select executable"),
                                            "",
                                            tr("Applications (*.exe);;All files (*.*)"));
#else
        return QFileDialog::getOpenFileName(this, tr("Select executable"), "/usr/bin");
#endif
    };

    ui->exiftoolPath->setBrowseButtonText(tr("Browse"));
    ui->exiftoolPath->setBrowseCallback(chooseExecutable);
}


ToolsTab::~ToolsTab()
{
    delete ui;
}


QtExtChooseFile* ToolsTab::exiftoolPath() const
{
    return ui->exiftoolPath;
}


ToolsTabController::ToolsTabController()
{
}


ToolsTabController::~ToolsTabController()
{
}


int ToolsTabController::tabId() const
{
    return 20;
}


QString ToolsTabController::tabName() const
{
    return tr("External tools");
}


QWidget* ToolsTabController::constructTab()
{
    ToolsTab* tab = new ToolsTab;
    setTabWidget(tab);

    IConfiguration* config = configuration();

    const QString exiftoolPath = config->getEntry(ExternalToolsConfigKeys::exiftoolPath).toString();

    tab->exiftoolPath()->setValue(exiftoolPath);

    connect(tab, &QObject::destroyed, [this](QObject* t)
    {
        assert(t != nullptr);

        setTabWidget(nullptr);
    });

    return tab;
}


void ToolsTabController::applyConfiguration()
{
    ToolsTab* tab = tabWidget();

    const QString exiftoolPath = tab->exiftoolPath()->value();

    IConfiguration* config = configuration();

    config->setEntry(ExternalToolsConfigKeys::exiftoolPath, exiftoolPath);
}


void ToolsTabController::rejectConfiguration()
{
}
