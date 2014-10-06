
#include "mainwindow.hpp"

#include <QCloseEvent>
#include <QMenuBar>
#include <QFileDialog>

#include <core/base_tags.hpp>
#include <database/database_builder.hpp>
#include <database/idatabase.hpp>
#include <project_utils/iproject_manager.hpp>
#include <project_utils/iproject.hpp>

#include "components/project_creator/project_creator_dialog.hpp"
#include "components/browse_view/main_view_data_model.hpp"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *p): QMainWindow(p),
    ui(new Ui::MainWindow),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr),
    m_currentPrj(nullptr),
    m_imagesModel(nullptr)
{
    ui->setupUi(this);
    setupView();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::set(IProjectManager* prjManager)
{
    m_prjManager = prjManager;
}


void MainWindow::set(IPluginLoader* pluginLoader)
{
    m_pluginLoader = pluginLoader;
}


void MainWindow::set(ITaskExecutor* taskExecutor)
{
    m_imagesModel->set(taskExecutor);
}


void MainWindow::set(IConfiguration* configuration)
{
    ui->photoView->set(configuration);
}


void MainWindow::closeEvent(QCloseEvent *e)
{
    // TODO: close project!
    //m_currentPrj->close();

    e->accept();
}


void MainWindow::openProject(const QString& prjFile)
{
    if (prjFile.isEmpty() == false)
    {
        std::shared_ptr<IProject> prj = m_prjManager->open(prjFile);
        m_currentPrj = prj;
        Database::IDatabase* db = m_currentPrj->getDatabase();

        m_imagesModel->setDatabase(db);
    }
}


void MainWindow::setupView()
{
    m_imagesModel = new MainViewDataModel(this);
    ui->photoView->setModel(m_imagesModel);

    ui->sortingCombo->addItem("Date and time");
    ui->sortingCombo->addItem("People");
}


void MainWindow::on_actionNew_project_triggered()
{
    ProjectCreatorDialog prjCreatorDialog;
    prjCreatorDialog.set(m_pluginLoader);
    const int status = prjCreatorDialog.exec();

    if (status == QDialog::Accepted)
    {
        const QString prjPath   = prjCreatorDialog.getPrjPath();
        const auto*   prjPlugin = prjCreatorDialog.getEnginePlugin();

        const bool creation_status = m_prjManager->new_prj(prjPath, prjPlugin);

        if (creation_status)
            openProject(prjPath);
    }
}

void MainWindow::on_actionOpen_project_triggered()
{
    const QString prjFile = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                         "",
                                                         tr("Broom projects (*.bpj)"));

    openProject(prjFile);
}


void MainWindow::on_sortingCombo_currentIndexChanged(int index)
{
    if (index == 0)
    {
        Hierarchy hierarchy;
        hierarchy.levels = { { BaseTags::get(BaseTagsList::Date), Hierarchy::Level::Order::ascending }  };

        m_imagesModel->setHierarchy(hierarchy);
    }
    else
    {
        Hierarchy hierarchy;
        hierarchy.levels = { { BaseTags::get(BaseTagsList::People), Hierarchy::Level::Order::ascending }  };

        m_imagesModel->setHierarchy(hierarchy);
    }
}


void MainWindow::on_actionAdd_photos_triggered()
{

}
