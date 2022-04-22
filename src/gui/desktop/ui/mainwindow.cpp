
#include "mainwindow.hpp"

#include <functional>
#include <ranges>

#include <QFileDialog>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QtQuick/QQuickItem>
#include <QTimer>

#include <core/constants.hpp>
#include <core/iconfiguration.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/ifeatures_manager.hpp>
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include <core/media_types.hpp>
#include <core/observables_registry.hpp>
#include <core/task_executor_utils.hpp>
#include <database/database_builder.hpp>
#include <database/database_tools/photos_analyzer.hpp>
#include <database/idatabase.hpp>
#include <database/igroup_operator.hpp>
#include <database/photo_utils.hpp>
#include <database/database_executor_traits.hpp>
#include <project_utils/iproject_manager.hpp>
#include <project_utils/project.hpp>
#include <system/system.hpp>
#include <features.hpp>

#include "config.hpp"

#include "config_keys.hpp"
#include "config_tabs/main_tab.hpp"
#include "config_tabs/tools_tab.hpp"
#include "models/flat_model.hpp"
#include "widgets/project_creator/project_creator_dialog.hpp"
#include "widgets/series_detection/series_detection.hpp"
#include "widgets/collection_dir_scan_dialog.hpp"
#include "ui_utils/config_dialog_manager.hpp"
#include "utils/groups_manager.hpp"
#include "utils/grouppers/collage_generator.hpp"
#include "utils/model_index_utils.hpp"
#include "utils/qml_utils.hpp"
#include "quick_items/objects_accessor.hpp"
#include "quick_items/photos_model_controller_component.hpp"
#include "quick_items/selection_manager_component.hpp"
#include "quick_items/thumbnail_image_provider.hpp"


MainWindow::MainWindow(IFeaturesManager& featuresManager, ICoreFactoryAccessor* coreFactory, IThumbnailsManager* thbMgr, QWidget *p):
    QObject(p),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr),
    m_currentPrj(nullptr),
    m_configuration(coreFactory->getConfiguration()),
    m_loggerFactory(coreFactory->getLoggerFactory()),
    m_updater(nullptr),
    m_coreAccessor(coreFactory),
    m_thumbnailsManager(thbMgr),
    m_configDialogManager(new ConfigDialogManager),
    m_mainTabCtrl(new MainTabController),
    m_toolsTabCtrl(new ToolsTabController),
    m_completerFactory(m_loggerFactory),
    m_featuresObserver(featuresManager, m_notifications)
{
    // setup
    setupConfig();
    setupQmlView();
    updateGui();
    registerConfigTab();

    connect(this, &MainWindow::currentDatabaseChanged,
            &m_completerFactory, qOverload<Database::IDatabase *>(&CompleterFactory::set));
    connect(this, &MainWindow::currentDatabaseChanged,
            &ObjectsAccessor::instance(), &ObjectsAccessor::setDatabase);
    connect(this, &MainWindow::currentProjectChanged,
            &ObjectsAccessor::instance(), &ObjectsAccessor::setProject);

    m_mainTabCtrl->set(&m_configuration);
    m_toolsTabCtrl->set(&m_configuration);
}


MainWindow::~MainWindow()
{
    // TODO: close project!
    //m_currentPrj->close();

    closeProject();

    //store recent collections
    m_configuration.setEntry("gui::recent", ObjectsAccessor::instance().recentProjects().join(";"));
}


void MainWindow::set(IProjectManager* prjManager)
{
    m_prjManager = prjManager;
}


void MainWindow::set(IPluginLoader* pluginLoader)
{
    m_pluginLoader = pluginLoader;
}


void MainWindow::setupQmlView()
{
    qmlRegisterSingletonInstance("photo_broom.singletons", 1, 0, "PhotoBroomProject", &ObjectsAccessor::instance());
    qmlRegisterSingletonInstance("photo_broom.singletons", 1, 0, "ObservablesRegistry", &ObservablesRegistry::instance());

    m_mainView.load(QUrl("qrc:///photo_broom.items/Views/MainWindow.qml"));

    QObject* mainWindow = QmlUtils::findQmlObject(m_mainView, "MainWindow");
    connect(mainWindow, SIGNAL(newProject()), this, SLOT(on_actionNew_collection_triggered()));
    connect(mainWindow, SIGNAL(openProjectDialog()), this, SLOT(on_actionOpen_collection_triggered()));
    connect(mainWindow, SIGNAL(openProject(QString)), this, SLOT(openProject(QString)));
    connect(mainWindow, SIGNAL(closeProject()), this, SLOT(on_actionClose_triggered()));
    connect(mainWindow, SIGNAL(scanCollection()), this, SLOT(on_actionScan_collection_triggered()));
    connect(mainWindow, SIGNAL(seriesDetector()), this, SLOT(on_actionSeries_detector_triggered()));
    connect(mainWindow, SIGNAL(configuration()), this, SLOT(on_actionConfiguration_triggered()));

    QmlUtils::registerImageProviders(m_mainView, *m_thumbnailsManager);
    PhotosModelControllerComponent* controller
        = qobject_cast<PhotosModelControllerComponent *>(QmlUtils::findQmlObject(m_mainView, "photos_model_controller"));

    assert(controller != nullptr);

    controller->setCompleterFactory(&m_completerFactory);

    QObject* tasksView = QmlUtils::findQmlObject(m_mainView, "TasksView");
    QObject* notificationsList = QmlUtils::findQmlObject(m_mainView, "NotificationsList");

    tasksView->setProperty("model", QVariant::fromValue(&m_tasksModel));
    notificationsList->setProperty("model", QVariant::fromValue(&m_notifications));
}


void MainWindow::setupConfig()
{
    // setup defaults
    m_configuration.setDefaultValue(UpdateConfigKeys::updateEnabled,   true);

    loadRecentCollections();
}


void MainWindow::set(IUpdater* updater)
{
    m_updater = updater;

    const bool enabled = m_configuration.getEntry(UpdateConfigKeys::updateEnabled).toBool();

    if (enabled)
    {
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        const QVariant last_raw = m_configuration.getEntry(UpdateConfigKeys::lastCheck);
        const std::chrono::system_clock::duration last(last_raw.isValid()? last_raw.toLongLong() : 0);
        const std::chrono::system_clock::time_point last_check(last);

        const auto diff = std::chrono::duration_cast<std::chrono::hours>(now - last_check).count();

        // hardcoded refresh frequency - 72 hours
        const int freqHours = 71;

        if (diff > freqHours || diff < 0)    // negative diff may be a result of broken config or changed clock settings
        {
            QTimer::singleShot(10000, this, &MainWindow::checkVersion);

            const std::chrono::system_clock::duration now_duration = now.time_since_epoch();
            const QVariant now_duration_raw = QVariant::fromValue<long long>(now_duration.count());
            m_configuration.setEntry(UpdateConfigKeys::lastCheck, now_duration_raw);
        }
    }
}


void MainWindow::checkVersion()
{
    m_loggerFactory.get("Updater")->info("Checking for new version");

    auto callback = std::bind(&MainWindow::currentVersion, this, std::placeholders::_1);
    m_updater->getStatus(callback);
}


void MainWindow::currentVersion(const IUpdater::OnlineVersion& versionInfo)
{
    auto logger = m_loggerFactory.get("Updater");

    switch (versionInfo.status)
    {
        case IUpdater::OnlineVersion::NewVersionAvailable:
            logger->info("New version avialable");
            QMessageBox::information(nullptr,
                                     tr("New version"),
                                     tr("New version of PhotoBroom is available <a href=\"%1\">here</a>.")
                                        .arg(versionInfo.url.url())
                                    );
            break;

        case IUpdater::OnlineVersion::ConnectionError:
            logger->error("Error when checking for new version");
            QMessageBox::critical(nullptr,
                                  tr("Internet connection problem"),
                                  tr("Could not check if there is new version of PhotoBroom.\n"
                                     "Please check your internet connection.")
                                 );
            break;

        case IUpdater::OnlineVersion::UpToDate:
            logger->info("Application is up to date");
            break;
    }
}


void MainWindow::openProject(const QString& prjPath, bool is_new)
{
    const ProjectInfo prjInfo(prjPath);

    if (prjInfo.isValid())
    {
        closeProject();

        // setup search path prefix
        assert( QDir::searchPaths("prj").isEmpty() == true );
        QDir::setSearchPaths("prj", { prjInfo.getBaseDir() } );
        auto open_status = m_prjManager->open(prjInfo);

        m_currentPrj = std::move(open_status.first);
        projectOpened(open_status.second, is_new);

        // add project to list of recent projects
        QStringList projects = ObjectsAccessor::instance().recentProjects();
        projects.removeAll(prjInfo.getPath());  // remove entry if it alredy exists
        projects.prepend(prjInfo.getPath());    // add it at the beginning
        ObjectsAccessor::instance().setRecentProjects(projects);
    }
}


void MainWindow::closeProject()
{
    if (m_currentPrj)
    {
        // Move m_currentPrj to a temporary place, so m_currentPrj is null and all tools will change theirs state basing on this.
        // Project object will be destroyed at the end of this routine
        auto prj = std::move(m_currentPrj);

        emit currentDatabaseChanged(nullptr);
        emit currentProjectChanged(nullptr);

        updateGui();

        QDir::setSearchPaths("prj", QStringList() );
    }
}


void MainWindow::updateGui()
{
    updateTools();
    updateProjectProperties();
}


void MainWindow::updateTools()
{
    const bool prj = m_currentPrj.get() != nullptr;

    if (prj)
    {
        m_photosAnalyzer = std::make_unique<PhotosAnalyzer>(m_coreAccessor, m_currentPrj->getDatabase());
        m_photosAnalyzer->set(&m_tasksModel);
        m_thumbnailsManager->setDatabaseCache(&m_currentPrj->getDatabase());
    }
    else
    {
        m_photosAnalyzer.reset();
        m_thumbnailsManager->setDatabaseCache(nullptr);
    }
}


void MainWindow::updateProjectProperties()
{
    const bool prj = m_currentPrj.get() != nullptr;

    QObject* mainWindow = QmlUtils::findQmlObject(m_mainView, "MainWindow");
    mainWindow->setProperty("projectOpened", QVariant::fromValue(prj));
    mainWindow->setProperty("projectName", QVariant::fromValue(prj? m_currentPrj->getProjectInfo().getName(): QString()));
}


void MainWindow::registerConfigTab()
{
    m_configDialogManager->registerTab(m_mainTabCtrl.get());
    m_configDialogManager->registerTab(m_toolsTabCtrl.get());
}


void MainWindow::loadRecentCollections()
{
    // recent collections
    const QString rawList = m_configuration.getEntry("gui::recent").toString();

    if (rawList.isEmpty() == false)
        ObjectsAccessor::instance().setRecentProjects(rawList.split(";"));
}


void MainWindow::on_actionNew_collection_triggered()
{
    ProjectCreator prjCreator;
    const bool creation_status = prjCreator.create(m_prjManager, m_pluginLoader);

    if (creation_status)
        openProject(prjCreator.project().getPath(), true);
}


void MainWindow::on_actionOpen_collection_triggered()
{
    const QString prjPath = QFileDialog::getOpenFileName(nullptr, tr("Open collection"), QString(), tr("Photo Broom files (*.bpj)"));

    if (prjPath.isEmpty() == false)
        openProject(prjPath);
}


void MainWindow::on_actionClose_triggered()
{
    closeProject();
}


void MainWindow::on_actionScan_collection_triggered()
{
    Database::IDatabase& db = m_currentPrj->getDatabase();

    CollectionDirScanDialog scanner(m_currentPrj.get(), db);
    const int status = scanner.exec();

    if (status == QDialog::Accepted)
    {
        const std::set<QString>& paths = scanner.newPhotos();

        std::vector<Photo::DataDelta> photos;
        for(const QString& path: paths)
        {
            const Photo::FlagValues flags = { {Photo::FlagsE::StagingArea, 1} };

            Photo::DataDelta photo_data;
            photo_data.insert<Photo::Field::Path>(path);
            photo_data.insert<Photo::Field::Flags>(flags);
            photos.emplace_back(photo_data);
        }

        db.exec([photos](Database::IBackend& backend) mutable
        {
            backend.addPhotos(photos);
        });
    }
}


void MainWindow::on_actionHelp_triggered()
{

}


void MainWindow::on_actionAbout_triggered()
{
    QString about;
    about =  QString("Photo Broom ") + PHOTO_BROOM_VERSION + "\n";
    about += "by Michał Walenciak";

    QMessageBox::about(nullptr, tr("About Photo Broom"), about);
}


void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(nullptr, tr("About Qt"));
}


void MainWindow::on_actionSeries_detector_triggered()
{
    SeriesDetection{m_currentPrj->getDatabase(), m_coreAccessor, m_tasksModel, *m_currentPrj.get(), *m_thumbnailsManager}.exec();
}


void MainWindow::on_actionConfiguration_triggered()
{
    m_configDialogManager->run();
}


void MainWindow::projectOpened(const Database::BackendStatus& status, bool is_new)
{
    switch(status.get())
    {
        case Database::StatusCodes::Ok:
        {
            Database::IDatabase& db = m_currentPrj->getDatabase();

            emit currentDatabaseChanged(&db);
            emit currentProjectChanged(m_currentPrj.get());

            // TODO: I do not like this flag here...
            if (is_new)
                on_actionScan_collection_triggered();
            break;
        }

        case Database::StatusCodes::BadVersion:
            QMessageBox::critical(nullptr,
                                  tr("Unsupported photo collection version"),
                                  tr("Photo collection you are trying to open uses database in version which is not supported.\n"
                                     "It means your application is too old to open it.\n\n"
                                     "Please upgrade application to open this collection.")
                                 );
            closeProject();
            break;

        case Database::StatusCodes::VersionTooOld:
            QMessageBox::critical(nullptr,
                                  tr("Unsupported photo collection version"),
                                  tr("Photo collection you are trying to open uses database in version which is not supported.\n"
                                     "It means your database is too old to open it.\n\n")
                                 );
            closeProject();
            break;

        case Database::StatusCodes::OpenFailed:
            QMessageBox::critical(nullptr,
                                  tr("Could not open collection"),
                                  tr("Photo collection could not be opened.\n"
                                     "It usually means that collection files are broken\n"
                                     "or you don't have rights to access them.\n\n"
                                     "Please check collection files:\n%1").arg(m_currentPrj->getProjectInfo().getPath())
                                 );
            closeProject();
            break;

        case Database::StatusCodes::ProjectLocked:
            QMessageBox::critical(nullptr,
                                  tr("Collection locked"),
                                  tr("Photo collection could not be opened.\n"
                                     "It is already opened by another Photo Broom instance.")
                                  );
            closeProject();
            break;

        default:
            QMessageBox::critical(nullptr,
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
