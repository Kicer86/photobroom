
#include "mainwindow.hpp"
#include "project_picker.hpp"

#include <functional>

#include <QCloseEvent>
#include <QMenuBar>
#include <QFileDialog>
#include <QLayout>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>

#include <configuration/iconfiguration.hpp>
#include <database/database_builder.hpp>
#include <database/idatabase.hpp>
#include <database/database_tools/photos_analyzer.hpp>
#include <project_utils/iproject_manager.hpp>
#include <project_utils/iproject.hpp>

#include "config.hpp"

#include "widgets/info_widget.hpp"
#include "widgets/project_creator/project_creator_dialog.hpp"
#include "widgets/photos_data_model.hpp"
#include "widgets/staged_photos_data_model.hpp"
#include "widgets/photos_widget.hpp"
#include "widgets/staged_photos_widget.hpp"
#include "utils/photos_collector.hpp"
#include "utils/info_generator.hpp"
#include "ui/photos_add_dialog.hpp"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *p): QMainWindow(p),
    ui(new Ui::MainWindow),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr),
    m_currentPrj(nullptr),
    m_imagesModel(nullptr),
    m_stagedImagesModel(nullptr),
    m_configuration(nullptr),
    m_updater(nullptr),
    m_photosCollector(new PhotosCollector(this)),
    m_views(),
    m_photosAnalyzer(new PhotosAnalyzer),
    m_infoGenerator(new InfoGenerator(this))
{
    qRegisterMetaType<Database::BackendStatus >("Database::BackendStatus ");
    connect(this, SIGNAL(projectOpenedSignal(const Database::BackendStatus &)), this, SLOT(projectOpened(const Database::BackendStatus &)));
    connect(m_infoGenerator.get(), &InfoGenerator::infoUpdated, this, &MainWindow::updateInfoWidget);

    ui->setupUi(this);
    setupView();

    createMenus();
    updateGui();
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
    m_photosAnalyzer->set(taskExecutor);
}


void MainWindow::set(IConfiguration* configuration)
{
    m_configuration = configuration;
    m_photosAnalyzer->set(configuration);

    for(IView* view: m_views)
        view->set(configuration);

    loadGeometry();
}


void MainWindow::set(IUpdater* updater)
{
    m_updater = updater;

    QTimer::singleShot(1000, this, &MainWindow::checkVersion);
}


void MainWindow::checkVersion()
{
    auto callback = std::bind(&MainWindow::currentVersion, this, std::placeholders::_1);
    m_updater->getStatus(callback);
}


void MainWindow::currentVersion(const IUpdater::OnlineVersion& versionInfo)
{
    switch (versionInfo.status)
    {
        case IUpdater::OnlineVersion::NewVersionAvailable:
            QMessageBox::information(this,
                                     tr("New version"),
                                     tr("New version of PhotoBroom is available <a href=\"%1\">here</a>.")
                                        .arg(versionInfo.url.url())
                                    );
            break;

        case IUpdater::OnlineVersion::ConnectionError:
            QMessageBox::critical(this,
                                  tr("Internet connection problem"),
                                  tr("Could not check if there is new version of PhotoBroom.\n"
                                     "Please check your internet connection.")
                                 );
            break;

        default:
            break;
    }
}


void MainWindow::closeEvent(QCloseEvent *e)
{
    // TODO: close project!
    //m_currentPrj->close();

    closeProject();
    m_photosAnalyzer->stop();

    e->accept();

    // store windows state
    const QByteArray geometry = saveGeometry();
    m_configuration->setEntry("gui::geometry", geometry.toBase64());

    const QByteArray state = saveState();
    m_configuration->setEntry("gui::state", state.toBase64());
}


void MainWindow::openProject(const ProjectInfo& prjInfo)
{
    if (prjInfo.isValid())
    {
        closeProject();

        auto openCallback = std::bind(&MainWindow::projectOpenedNotification, this, std::placeholders::_1);

        m_currentPrj = m_prjManager->open(prjInfo, openCallback);
    }
}


void MainWindow::closeProject()
{
    if (m_currentPrj)
    {
        // Move m_currentPrj to a temporary place, so m_currentPrj is null and all tools will change theirs state basing on this.
        // Project object will be destroyed at the end of this routine
        auto prj = std::move(m_currentPrj);

        m_imagesModel->setDatabase(nullptr);
        m_stagedImagesModel->setDatabase(nullptr);
        m_infoGenerator->set(nullptr);

        updateGui();
    }
}


void MainWindow::setupView()
{
    m_imagesModel = new PhotosDataModel(this);
    PhotosWidget* photosWidget = new PhotosWidget(this);
    photosWidget->setWindowTitle(tr("Photos"));
    photosWidget->setModel(m_imagesModel);
    m_views.push_back(photosWidget);
    ui->viewsContainer->addWidget(photosWidget);

    m_stagedImagesModel = new StagedPhotosDataModel(this);
    StagedPhotosWidget* stagedPhotosWidget = new StagedPhotosWidget(this);
    stagedPhotosWidget->setWindowTitle(tr("Staged photos"));
    stagedPhotosWidget->setModel(m_stagedImagesModel);
    m_views.push_back(stagedPhotosWidget);
    ui->viewsContainer->addWidget(stagedPhotosWidget);

    ui->infoDockWidget->hide();

    //photos collector will write to stagedPhotosArea
    m_photosCollector->set(m_stagedImagesModel);
    m_photosCollector->set(ui->tasksWidget);

    m_photosAnalyzer->set(ui->tasksWidget);

    //
    viewChanged();

    QTimer::singleShot(0, m_infoGenerator.get(), &InfoGenerator::externalRefresh);
}


void MainWindow::createMenus()
{
    for(size_t i = 0; i < m_views.size(); i++)
    {
        IView* view = m_views[i];
        const QString title = view->getName();
        QAction* action = ui->menuWindows->addAction(title);

        action->setData(static_cast<int>(i));
        connect(ui->menuWindows, SIGNAL(triggered(QAction *)), this, SLOT(activateWindow(QAction*)));
    }
}


void MainWindow::updateMenus()
{
    const bool prj = m_currentPrj.get() != nullptr;

    ui->menuPhotos->menuAction()->setVisible(prj);
    ui->menuWindows->menuAction()->setVisible(prj);
}


void MainWindow::updateTitle()
{
    const bool prj = m_currentPrj.get() != nullptr;
    const QString title = tr("Photo broom: ") + (prj? m_currentPrj->getName(): tr("No collection opened"));

    setWindowTitle(title);
}


void MainWindow::updateGui()
{
    updateMenus();
    updateTitle();
    updateTools();
}


void MainWindow::updateTools()
{
    const bool prj = m_currentPrj.get() != nullptr;

    if (prj)
        m_photosAnalyzer->setDatabase(m_currentPrj->getDatabase());
    else
        m_photosAnalyzer->setDatabase(nullptr);
}


void MainWindow::viewChanged()
{
    const int w = ui->viewsContainer->currentIndex();

    IView* view = m_views[w];
    ui->tagEditor->set( view->getSelectionModel() );
    ui->tagEditor->set( view->getModel() );
}


void MainWindow::loadGeometry()
{
    // restore state
    const QVariant geometry = m_configuration->getEntry("gui::geometry");
    if (geometry.isValid())
    {
        const QByteArray base64 = geometry.toByteArray();
        const QByteArray geometryData = QByteArray::fromBase64(base64);
        restoreGeometry(geometryData);
    }

    const QVariant state = m_configuration->getEntry("gui::state");
    if (state.isValid())
    {
        const QByteArray base64 = state.toByteArray();
        const QByteArray stateData = QByteArray::fromBase64(base64);
        restoreState(stateData);
    }
}


void MainWindow::on_actionNew_collection_triggered()
{
    ProjectCreator prjCreator;
    const bool creation_status = prjCreator.create(m_prjManager, m_pluginLoader);

    if (creation_status)
        openProject(prjCreator.project());
}


void MainWindow::on_actionOpen_collection_triggered()
{
    ProjectPicker picker;

    picker.set(m_pluginLoader);
    picker.set(m_prjManager);
    const int s = picker.exec();

    if (s == QDialog::Accepted)
    {
        const ProjectInfo prjName = picker.choosenProject();

        openProject(prjName);
    }
}


void MainWindow::on_actionClose_triggered()
{
    closeProject();
}


void MainWindow::on_actionQuit_triggered()
{
    close();
}


void MainWindow::on_actionAdd_photos_triggered()
{
    PhotosAddDialog photosAddDialog;
    photosAddDialog.show();

    QEventLoop loop;

    connect(&photosAddDialog, &PhotosAddDialog::closing, &loop, &QEventLoop::quit);

    loop.exec();

    /*
    const QString path = QFileDialog::getExistingDirectory(this, tr("Choose directory with photos"));

    if (path.isEmpty() == false)
        m_photosCollector->addDir(path);
    */
}


void MainWindow::activateWindow(QAction* action)
{
    const int w = action->data().toInt();

    ui->viewsContainer->setCurrentIndex(w);

    viewChanged();
}


void MainWindow::on_actionHelp_triggered()
{

}


void MainWindow::on_actionAbout_triggered()
{
    QString about;
    about =  QString("Photo Broom ") + PHOTO_BROOM_VERSION + "\n";
    about += "by Michał Walenciak";

    QMessageBox::about(this, tr("About Photo Broom"), about);
}


void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}


void MainWindow::projectOpened(const Database::BackendStatus& status)
{
    switch(status.get())
    {
        case Database::StatusCodes::Ok:
        {
            Database::IDatabase* db = m_currentPrj->getDatabase();

            m_imagesModel->setDatabase(db);
            m_stagedImagesModel->setDatabase(db);
            m_infoGenerator->set(db);
            break;
        }

        case Database::StatusCodes::BadVersion:
            QMessageBox::critical(this,
                                  tr("Unsupported photo collection version"),
                                  tr("Photo collection you are trying to open uses database in version which is not supported.\n"
                                     "It means your application is too old to open it.\n\n"
                                     "Please upgrade application to open this collection.")
                                 );
            closeProject();
            break;

        case Database::StatusCodes::OpenFailed:
            QMessageBox::critical(this,
                                  tr("Could not open collection"),
                                  tr("Photo collection could not be opened.\n"
                                     "It usually means that collection files are broken\n"
                                     "or you don't have rights to access them.\n\n"
                                     "Please check collection files:\n%1").arg(m_currentPrj->getPrjPath())
                                 );
            closeProject();
            break;

        default:
            QMessageBox::critical(this,
                                  tr("Unexpected error"),
                                  tr("An unexpected error occured while opening photo collection.\n"
                                     "Please report a bug.\n"
                                     "Error code: %1").arg(static_cast<int>( status.get()) )
                                 );
            closeProject();
            break;
    }

    updateGui();
}


void MainWindow::updateInfoWidget(const QString& infoText)
{
    if (infoText.isEmpty() == false)
        ui->infoWidget->setText(infoText);

    if (infoText.isEmpty() && ui->infoDockWidget->isVisible())
        ui->infoDockWidget->hide();

    if (infoText.isEmpty() == false && ui->infoDockWidget->isHidden())
        ui->infoDockWidget->show();
}


void MainWindow::projectOpenedNotification(const Database::BackendStatus& status)
{
    emit projectOpenedSignal(status);
}

