
#include "mainwindow.hpp"

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
#include <project_utils/project.hpp>

#include "config.hpp"

#include "config_keys.hpp"
#include "config_tabs/look_tab.hpp"
#include "config_tabs/main_tab.hpp"
#include "models/photos_data_model.hpp"
#include "models/staged_photos_data_model.hpp"
#include "widgets/info_widget.hpp"
#include "widgets/project_creator/project_creator_dialog.hpp"
#include "widgets/photos_widget.hpp"
#include "widgets/collection_dir_scan_dialog.hpp"
#include "utils/config_dialog_manager.hpp"
#include "utils/photos_collector.hpp"
#include "utils/icons_loader.hpp"
#include "ui/photos_add_dialog.hpp"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *p): QMainWindow(p),
    ui(new Ui::MainWindow),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr),
    m_currentPrj(nullptr),
    m_imagesModel(nullptr),
    m_configuration(nullptr),
    m_updater(nullptr),
    m_executor(nullptr),
    m_photosManager(nullptr),
    m_photosAnalyzer(new PhotosAnalyzer),
    m_configDialogManager(new ConfigDialogManager),
    m_mainTabCtrl(new MainTabControler),
    m_lookTabCtrl(new LookTabControler)
{
    qRegisterMetaType<Database::BackendStatus>("Database::BackendStatus");
    connect(this, SIGNAL(projectOpenedSignal(const Database::BackendStatus &)), this, SLOT(projectOpened(const Database::BackendStatus &)));

    ui->setupUi(this);
    setupView();
    updateGui();
    registerConfigTab();

    IconsLoader icons;

    ui->actionNew_collection->setIcon(icons.getIcon(IconsLoader::Icon::New));
    ui->actionOpen_collection->setIcon(icons.getIcon(IconsLoader::Icon::Open));
    ui->actionClose->setIcon(icons.getIcon(IconsLoader::Icon::Close));
    ui->actionQuit->setIcon(icons.getIcon(IconsLoader::Icon::Quit));

    ui->actionConfiguration->setIcon(icons.getIcon(IconsLoader::Icon::Settings));

    ui->actionHelp->setIcon(icons.getIcon(IconsLoader::Icon::Help));
    ui->actionAbout->setIcon(icons.getIcon(IconsLoader::Icon::About));
    ui->actionAbout_Qt->setIcon(icons.getIcon(IconsLoader::Icon::AboutQt));
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
    m_photosAnalyzer->set(taskExecutor);

    m_executor = taskExecutor;
}


void MainWindow::set(IConfiguration* configuration)
{
    m_configuration = configuration;

    // setup defaults
    m_configuration->setDefaultValue(UpdateConfigKeys::updateEnabled,   true);
    m_configuration->setDefaultValue(UpdateConfigKeys::updateFrequency, 1);

    m_configuration->setDefaultValue(ViewConfigKeys::itemsMargin,    10);
    m_configuration->setDefaultValue(ViewConfigKeys::itemsSpacing,   2);
    m_configuration->setDefaultValue(ViewConfigKeys::thumbnailWidth, 120);
    m_configuration->setDefaultValue(ViewConfigKeys::bkg_color_even, 0xff000040u);
    m_configuration->setDefaultValue(ViewConfigKeys::bkg_color_odd,  0x0000ff40u);

    //
    m_mainTabCtrl->set(configuration);
    m_lookTabCtrl->set(configuration);
    m_photosAnalyzer->set(configuration);
    ui->imagesView->set(configuration);

    loadGeometry();
}


void MainWindow::set(IUpdater* updater)
{
    m_updater = updater;
    assert(m_configuration != nullptr);

    const bool enabled = m_configuration->getEntry(UpdateConfigKeys::updateEnabled).toBool();

    if (enabled)
    {
        const int frequency = m_configuration->getEntry(UpdateConfigKeys::updateFrequency).toInt() % 3;

        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        const QVariant last_raw = m_configuration->getEntry(UpdateConfigKeys::lastCheck);
        const std::chrono::system_clock::duration last(last_raw.isValid()? last_raw.toLongLong() : 0);
        const std::chrono::system_clock::time_point last_check(last);

        const auto diff = std::chrono::duration_cast<std::chrono::hours>(now - last_check).count();

        const int freqHours[] = {23, 71, 167};

        if (diff > freqHours[frequency] || diff < 0)    // negative diff may be a result of broken config or changed clock settings
        {
            QTimer::singleShot(10000, this, &MainWindow::checkVersion);

            std::chrono::system_clock::duration now_duration = now.time_since_epoch();
            const QVariant now_duration_raw = QVariant::fromValue<long long>(now_duration.count());
            m_configuration->setEntry(UpdateConfigKeys::lastCheck, now_duration_raw);
        }
    }
}


void MainWindow::set(IPhotosManager* manager)
{
    m_photosAnalyzer->set(manager);
    m_photosManager = manager;
}


void MainWindow::checkVersion()
{
    auto callback = std::bind(&MainWindow::currentVersion, this, std::placeholders::_1);
    m_updater->getStatus(callback);
}


void MainWindow::updateViewMenu()
{
    ui->actionTags_editor->setChecked(ui->rightDockWidget->isVisible());
    ui->actionTasks->setChecked(ui->tasksDockWidget->isVisible());
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

        case IUpdater::OnlineVersion::UpToDate:
            // nothing to do
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

        updateGui();
    }
}


void MainWindow::setupView()
{
    m_imagesModel = new PhotosDataModel(this);
    ui->imagesView->setModel(m_imagesModel);

    m_photosAnalyzer->set(ui->tasksWidget);

    //setup tags editor
    ui->tagEditor->set( ui->imagesView->viewSelectionModel() );
    ui->tagEditor->set( m_imagesModel);

    //connect to docks
    connect(ui->rightDockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(updateViewMenu()));
    connect(ui->tasksDockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(updateViewMenu()));
}


void MainWindow::updateMenus()
{
    const bool prj = m_currentPrj.get() != nullptr;

    ui->menuPhotos->menuAction()->setVisible(prj);
}


void MainWindow::updateTitle()
{
    const bool prj = m_currentPrj.get() != nullptr;
    const QString title = tr("Photo broom: ") + (prj? m_currentPrj->getProjectInfo().getName(): tr("No collection opened"));

    setWindowTitle(title);
}


void MainWindow::updateGui()
{
    updateMenus();
    updateTitle();
    updateTools();
    updateWidgets();
}


void MainWindow::updateTools()
{
    const bool prj = m_currentPrj.get() != nullptr;

    if (prj)
        m_photosAnalyzer->setDatabase(m_currentPrj->getDatabase());
    else
        m_photosAnalyzer->setDatabase(nullptr);
}


void MainWindow::updateWidgets()
{
    const bool prj = m_currentPrj.get() != nullptr;

    if (prj)
    {
        ui->imagesView->setEnabled(true);
        ui->tagEditor->setEnabled(true);
    }
    else
    {
        ui->imagesView->setDisabled(true);
        ui->tagEditor->setDisabled(true);
    }
}


void MainWindow::registerConfigTab()
{
    m_configDialogManager->registerTab(m_mainTabCtrl.get());
    m_configDialogManager->registerTab(m_lookTabCtrl.get());
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
    const QString prjPath = QFileDialog::getOpenFileName(this, tr("Open collection"), QString(), tr("Photo Broom files (*.bpj)"));

    if (prjPath.isEmpty() == false)
    {
        const ProjectInfo prjName(prjPath);

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
    PhotosAddDialog photosAddDialog(m_configuration);

    photosAddDialog.set(m_executor);
    photosAddDialog.set(m_currentPrj->getDatabase());
    photosAddDialog.set(m_photosManager);
    photosAddDialog.setWindowModality(Qt::ApplicationModal);
    photosAddDialog.exec();
}


void MainWindow::on_actionScan_collection_triggered()
{
    CollectionDirScanDialog().exec();
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


void MainWindow::on_actionTags_editor_triggered()
{
    const bool state = ui->actionTags_editor->isChecked();

    ui->rightDockWidget->setVisible(state);
}


void MainWindow::on_actionTasks_triggered()
{
    const bool state = ui->actionTasks->isChecked();

    ui->tasksDockWidget->setVisible(state);
}


void MainWindow::on_actionConfiguration_triggered()
{
    m_configDialogManager->run();
}


void MainWindow::projectOpened(const Database::BackendStatus& status)
{
    switch(status.get())
    {
        case Database::StatusCodes::Ok:
        {
            Database::IDatabase* db = m_currentPrj->getDatabase();

            m_imagesModel->setDatabase(db);
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
                                     "Please check collection files:\n%1").arg(m_currentPrj->getProjectInfo().getPath())
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


void MainWindow::projectOpenedNotification(const Database::BackendStatus& status)
{
    emit projectOpenedSignal(status);
}

