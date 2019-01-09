
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

    ui->aisPath->setBrowseButtonText(tr("Browse"));
    ui->aisPath->setBrowseCallback(chooseExecutable);
    ui->convertPath->setBrowseButtonText(tr("Browse"));
    ui->convertPath->setBrowseCallback(chooseExecutable);
    ui->ffmpegPath->setBrowseButtonText(tr("Browse"));
    ui->ffmpegPath->setBrowseCallback(chooseExecutable);
    ui->ffprobePath->setBrowseButtonText(tr("Browse"));
    ui->ffprobePath->setBrowseCallback(chooseExecutable);
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


QtExtChooseFile* ToolsTab::ffmpegPath() const
{
    return ui->ffmpegPath;
}


QtExtChooseFile* ToolsTab::ffprobePath() const
{
    return ui->ffprobePath;
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

    const QString aisPath = config->getEntry(ExternalToolsConfigKeys::aisPath).toString();
    const QString convertPath = config->getEntry(ExternalToolsConfigKeys::convertPath).toString();
    const QString ffmpegPath = config->getEntry(ExternalToolsConfigKeys::ffmpegPath).toString();
    const QString ffprobePath = config->getEntry(ExternalToolsConfigKeys::ffprobePath).toString();

    tab->aisPath()->setValue(aisPath);
    tab->convertPath()->setValue(convertPath);
    tab->ffmpegPath()->setValue(ffmpegPath);
    tab->ffprobePath()->setValue(ffprobePath);

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

    const QString aisPath = tab->aisPath()->value();
    const QString convertPath = tab->convertPath()->value();
    const QString ffmpegPath = tab->ffmpegPath()->value();
    const QString ffprobePath = tab->ffprobePath()->value();

    IConfiguration* config = configuration();

    config->setEntry(ExternalToolsConfigKeys::aisPath, aisPath);
    config->setEntry(ExternalToolsConfigKeys::convertPath, convertPath);
    config->setEntry(ExternalToolsConfigKeys::ffmpegPath, ffmpegPath);
    config->setEntry(ExternalToolsConfigKeys::ffprobePath, ffprobePath);
}


void ToolsTabController::rejectConfiguration()
{
}
