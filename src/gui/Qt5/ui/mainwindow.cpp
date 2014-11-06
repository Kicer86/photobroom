
#include "mainwindow.hpp"
#include "project_picker.hpp"

#include <QCloseEvent>
#include <QMenuBar>
#include <QFileDialog>
#include <QLayout>

#include <database/database_builder.hpp>
#include <database/idatabase.hpp>
#include <project_utils/iproject_manager.hpp>
#include <project_utils/iproject.hpp>

#include "components/project_creator/project_creator_dialog.hpp"
#include "components/photos_data_model.hpp"
#include "components/staged_photos_data_model.hpp"
#include "data/photos_collector.hpp"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *p): QMainWindow(p),
    ui(new Ui::MainWindow),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr),
    m_currentPrj(nullptr),
    m_imagesModel(nullptr),
    m_stagedImagesModel(nullptr),
    m_configuration(nullptr),
    m_photosCollector(new PhotosCollector(this))
{
    ui->setupUi(this);
    setupView();
    updateMenus();
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
    m_stagedImagesModel->set(taskExecutor);
}


void MainWindow::set(IConfiguration* configuration)
{
    m_configuration = configuration;
    ui->photoView->set(configuration);
    ui->stagedPhotosArea->set(configuration);
}


void MainWindow::closeEvent(QCloseEvent *e)
{
    // TODO: close project!
    //m_currentPrj->close();

    e->accept();
}


void MainWindow::openProject(const QString& prjName)
{
    if (prjName.isEmpty() == false)
    {
        std::shared_ptr<IProject> prj = m_prjManager->open(prjName);
        m_currentPrj = prj;
        Database::IDatabase* db = m_currentPrj->getDatabase();

        m_imagesModel->setDatabase(db);
        m_stagedImagesModel->setDatabase(db);
    }

    updateMenus();
}


void MainWindow::setupView()
{
    m_imagesModel = new PhotosDataModel(this);
    ui->photoView->setModel(m_imagesModel);

    m_stagedImagesModel = new StagedPhotosDataModel(this);
    ui->stagedPhotosArea->setModel(m_stagedImagesModel);

    //photos collector will write to stagedPhotosArea
    m_photosCollector->set(m_stagedImagesModel);
}


void MainWindow::updateMenus()
{
    const bool prj = m_currentPrj.get() != nullptr;
    QStackedWidget* centerWidget = ui->centralWidget;

    ui->menuPhotos->menuAction()->setVisible(prj);

    //remove any windows from "Windows" menu
    ui->menuWindows->clear();
    ui->menuWindows->disconnect(ui->menuWindows);
    ui->menuWindows->menuAction()->setVisible(prj);

    //reattach items to "Windows" menu
    const int c = centerWidget->layout()->count();
    for(int i = 0; i < c; i++)
    {
        QWidget* child = centerWidget->widget(i);
        assert(child != nullptr);

        const QString title = child->windowTitle();
        QAction* action = ui->menuWindows->addAction(title);

        action->setData(i);
        connect(ui->menuWindows, SIGNAL(triggered(QAction *)), this, SLOT(activateWindow(QAction*)));
    }
}


void MainWindow::on_actionNew_project_triggered()
{
    ProjectCreator prjCreator;
    const bool creation_status = prjCreator.create(m_prjManager, m_pluginLoader);

    if (creation_status)
        openProject(prjCreator.prjName());
}

void MainWindow::on_actionOpen_project_triggered()
{
    ProjectPicker picker;

    picker.set(m_pluginLoader);
    picker.set(m_prjManager);
    const int s = picker.exec();
    
    if (s == QDialog::Accepted)
    {
        const QString prjName = picker.choosenProjectName();

        openProject(prjName);
    }
}


void MainWindow::on_actionAdd_photos_triggered()
{
    const QString path = QFileDialog::getExistingDirectory(this, tr("Choose directory with photos"));

    if (path.isEmpty() == false)
        m_photosCollector->addDir(path);
}


void MainWindow::activateWindow(QAction* action)
{
    const int w = action->data().toInt();

    ui->centralWidget->setCurrentIndex(w);
}
