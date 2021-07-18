
#include "mainwindow.hpp"

#include <functional>
#include <ranges>

#include <QCloseEvent>
#include <QDesktopServices>
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
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include <core/media_types.hpp>
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
#include "utils/selection_to_photoid_translator.hpp"
#include "utils/model_index_utils.hpp"
#include "ui_utils/icons_loader.hpp"
#include "quick_views/qml_utils.hpp"
#include "quick_views/photos_model_controller_component.hpp"
#include "quick_views/selection_manager_component.hpp"
#include "ui_mainwindow.h"
#include "ui/faces_dialog.hpp"
#include "ui/photos_grouping_dialog.hpp"


MainWindow::MainWindow(ICoreFactoryAccessor* coreFactory, IThumbnailsManager* thbMgr, QWidget *p): QMainWindow(p),
    m_thumbnailsManager4QML(thbMgr),
    ui(new Ui::MainWindow),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr),
    m_currentPrj(nullptr),
    m_photosModelController(nullptr),
    m_configuration(coreFactory->getConfiguration()),
    m_loggerFactory(coreFactory->getLoggerFactory()),
    m_updater(nullptr),
    m_executor(coreFactory->getTaskExecutor()),
    m_coreAccessor(coreFactory),
    m_thumbnailsManager(thbMgr),
    m_configDialogManager(new ConfigDialogManager),
    m_mainTabCtrl(new MainTabController),
    m_toolsTabCtrl(new ToolsTabController),
    m_recentCollections(),
    m_completerFactory(m_loggerFactory),
    m_enableFaceRecognition(FaceRecognition::checkSystem())
{
    // setup
    ui->setupUi(this);
    setupConfig();
    setupView();
    updateGui();
    registerConfigTab();

    connect(this, &MainWindow::currentDatabaseChanged,
            m_photosModelController, &PhotosModelControllerComponent::setDatabase);
    connect(this, &MainWindow::currentDatabaseChanged,
            &m_completerFactory, qOverload<Database::IDatabase *>(&CompleterFactory::set));
    connect(this, &MainWindow::currentDatabaseChanged,
            ui->tagEditor, &TagEditorWidget::setDatabase);
    connect(this, &MainWindow::currentDatabaseChanged,
            &m_bridge, &Bridge::setDatabase);

    IconsLoader icons;

    ui->actionNew_collection->setIcon(icons.getIcon(IconsLoader::Icon::New));
    ui->actionOpen_collection->setIcon(icons.getIcon(IconsLoader::Icon::Open));
    ui->actionClose->setIcon(icons.getIcon(IconsLoader::Icon::Close));
    ui->actionQuit->setIcon(icons.getIcon(IconsLoader::Icon::Quit));

    ui->actionConfiguration->setIcon(icons.getIcon(IconsLoader::Icon::Settings));

    ui->actionHelp->setIcon(icons.getIcon(IconsLoader::Icon::Help));
    ui->actionAbout->setIcon(icons.getIcon(IconsLoader::Icon::About));
    ui->actionAbout_Qt->setIcon(icons.getIcon(IconsLoader::Icon::AboutQt));

    m_mainTabCtrl->set(&m_configuration);
    m_toolsTabCtrl->set(&m_configuration);

    ui->tagEditor->set(&m_completerFactory);

    // TODO: nothing useful in help menu at this moment
    ui->menuHelp->menuAction()->setVisible(false);

    if (m_enableFaceRecognition == false)
        m_loggerFactory.get("MainWindow")->warning("Face recognition cannot be enabled");
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


void MainWindow::setupQmlView()
{
    assert(m_photosModelController == nullptr);

    qmlRegisterSingletonInstance("photo_broom.qml", 1, 0, "PhotoBroomProject", &m_bridge);

    QmlUtils::registerObject(ui->mainViewQml, "thumbnailsManager", &m_thumbnailsManager4QML);
    ui->mainViewQml->setSource(QUrl("qrc:/ui/Dialogs/MainWindow.qml"));
    m_photosModelController = qobject_cast<PhotosModelControllerComponent *>(QmlUtils::findQmlObject(ui->mainViewQml, "photos_model_controller"));

    assert(m_photosModelController != nullptr);

    m_photosModelController->setCompleterFactory(&m_completerFactory);

    SelectionManagerComponent* selectionManager =
        qobject_cast<SelectionManagerComponent *>(QmlUtils::findQmlObject(ui->mainViewQml, "selectionManager"));

    m_selectionTranslator = std::make_unique<SelectionToPhotoDataTranslator>(*selectionManager, *m_photosModelController->model());

    SelectionChangeNotifier* translator = new SelectionChangeNotifier(*selectionManager, *m_selectionTranslator.get(), this);

    connect(translator, &SelectionChangeNotifier::selectionChanged, ui->tagEditor, &TagEditorWidget::editPhotos);
    connect(translator, &SelectionChangeNotifier::selectionChanged, ui->photoPropertiesWidget, &PhotoPropertiesWidget::setPhotos);

    QObject* notificationsList = QmlUtils::findQmlObject(ui->mainViewQml, "NotificationsList");
    notificationsList->setProperty("model", QVariant::fromValue(&m_notifications));
}


void MainWindow::setupConfig()
{
    // setup defaults
    m_configuration.setDefaultValue(UpdateConfigKeys::updateEnabled,   true);

    loadGeometry();
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


void MainWindow::updateWindowsMenu()
{
    ui->actionTags_editor->setChecked(ui->tagEditorDockWidget->isVisible());
    ui->actionTasks->setChecked(ui->tasksDockWidget->isVisible());
    ui->actionPhoto_properties->setChecked(ui->photoPropertiesDockWidget->isVisible());
}


void MainWindow::currentVersion(const IUpdater::OnlineVersion& versionInfo)
{
    auto logger = m_loggerFactory.get("Updater");

    switch (versionInfo.status)
    {
        case IUpdater::OnlineVersion::NewVersionAvailable:
            logger->info("New version avialable");
            QMessageBox::information(this,
                                     tr("New version"),
                                     tr("New version of PhotoBroom is available <a href=\"%1\">here</a>.")
                                        .arg(versionInfo.url.url())
                                    );
            break;

        case IUpdater::OnlineVersion::ConnectionError:
            logger->error("Error when checking for new version");
            QMessageBox::critical(this,
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


void MainWindow::closeEvent(QCloseEvent *e)
{
    // TODO: close project!
    //m_currentPrj->close();

    closeProject();

    e->accept();

    // store windows state
    const QByteArray geometry = saveGeometry();
    m_configuration.setEntry("gui::geometry", geometry.toBase64());

    const QByteArray state = saveState();
    m_configuration.setEntry("gui::state", state.toBase64());

    //store recent collections
    m_configuration.setEntry("gui::recent", m_recentCollections.join(";"));
}


void MainWindow::openProject(const ProjectInfo& prjInfo, bool is_new)
{
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
        m_recentCollections.removeAll(prjInfo.getPath());  // remove entry if it alredy exists

        m_recentCollections.prepend(prjInfo.getPath());    // add it at the beginning
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

        updateGui();

        QDir::setSearchPaths("prj", QStringList() );
    }
}


void MainWindow::setupView()
{
    setupQmlView();

    // connect to docks
    connect(ui->tagEditorDockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(updateWindowsMenu()));
    connect(ui->tasksDockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(updateWindowsMenu()));
    connect(ui->photoPropertiesDockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(updateWindowsMenu()));

    connect(ui->mainViewQml, &QWidget::customContextMenuRequested,
            this, &MainWindow::showContextMenu);
}


void MainWindow::updateMenus()
{
    const bool prj = m_currentPrj.get() != nullptr;
    const bool valid = m_recentCollections.isEmpty() == false;

    ui->menuPhotos->menuAction()->setVisible(prj);
    ui->menuTools->menuAction()->setVisible(prj);
    ui->menuOpen_recent->menuAction()->setVisible(valid);
    ui->menuOpen_recent->clear();

    for(const QString& entry: qAsConst(m_recentCollections))
    {
        QAction* action = ui->menuOpen_recent->addAction(entry);
        connect(action, &QAction::triggered, [=, this]
        {
            const ProjectInfo prjInfo(entry);

            openProject(prjInfo);
        });
    }
}


void MainWindow::updateTitle()
{
    const bool prj = m_currentPrj.get() != nullptr;

    const QString prjName = prj? m_currentPrj->getProjectInfo().getName(): tr("No collection opened");
    const QString title = tr("Photo broom: %1").arg(prjName);

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
    {
        m_photosAnalyzer = std::make_unique<PhotosAnalyzer>(m_coreAccessor, m_currentPrj->getDatabase());
        m_photosAnalyzer->set(ui->tasksWidget);
        m_thumbnailsManager->setDatabaseCache(&m_currentPrj->getDatabase());
    }
    else
    {
        m_photosAnalyzer.reset();
        m_thumbnailsManager->setDatabaseCache(nullptr);
    }
}


void MainWindow::updateWidgets()
{
    const bool prj = m_currentPrj.get() != nullptr;

    QObject* notificationsList = QmlUtils::findQmlObject(ui->mainViewQml, "MainWindow");
    notificationsList->setProperty("projectOpened", QVariant::fromValue(prj));

    ui->tagEditor->setEnabled(prj);
}


void MainWindow::registerConfigTab()
{
    m_configDialogManager->registerTab(m_mainTabCtrl.get());
    m_configDialogManager->registerTab(m_toolsTabCtrl.get());
}


void MainWindow::loadGeometry()
{
    // restore state
    const QVariant geometry = m_configuration.getEntry("gui::geometry");
    if (geometry.isValid())
    {
        const QByteArray base64 = geometry.toString().toLatin1();
        const QByteArray geometryData = QByteArray::fromBase64(base64);
        restoreGeometry(geometryData);
    }

    const QVariant state = m_configuration.getEntry("gui::state");
    if (state.isValid())
    {
        const QByteArray base64 = state.toByteArray();
        const QByteArray stateData = QByteArray::fromBase64(base64);
        restoreState(stateData);
    }
}


void MainWindow::loadRecentCollections()
{
    // recent collections
    const QString rawList = m_configuration.getEntry("gui::recent").toString();

    if (rawList.isEmpty() == false)
        m_recentCollections = rawList.split(";");

    updateMenus();
}


void MainWindow::showContextMenu(const QPoint& pos)
{
    const std::vector<Photo::Data> selected_photos = m_selectionTranslator->getSelectedDatas();

    std::vector<Photo::Data> photos;
    std::remove_copy_if(selected_photos.cbegin(),
                        selected_photos.cend(),
                        std::back_inserter(photos),
                        [](const Photo::Data& photo){
                            return QFile::exists(photo.path) == false;
                        });

    QMenu contextMenu;
    QAction* groupPhotos    = contextMenu.addAction(tr("Group"));
    QAction* manageGroup    = contextMenu.addAction(tr("Manage group..."));
    QAction* ungroupPhotos  = contextMenu.addAction(tr("Ungroup"));
    QAction* location       = contextMenu.addAction(tr("Open photo location"));
    QAction* faces          = contextMenu.addAction(tr("Recognize people..."));

    const bool groupsOnly = std::ranges::all_of(photos, [](const Photo::Data& photo) { return photo.groupInfo.role == GroupInfo::Role::Representative; });
    const bool isSingleGroup = photos.size() == 1 && groupsOnly;

    groupPhotos->setEnabled(photos.size() > 1 && std::ranges::all_of(photos | std::views::transform(&Photo::getPath), &MediaTypes::isImageFile));
    manageGroup->setEnabled(isSingleGroup);
    ungroupPhotos->setEnabled(groupsOnly);
    location->setEnabled(photos.size() == 1);
    faces->setEnabled(m_enableFaceRecognition && photos.size() == 1 && MediaTypes::isImageFile(photos.front().path));

    ungroupPhotos->setVisible(groupsOnly);

    Database::IDatabase& db = m_currentPrj->getDatabase();

    const QPoint globalPos = ui->mainViewQml->mapToGlobal(pos);
    QAction* chosenAction = contextMenu.exec(globalPos);

    if (chosenAction == groupPhotos)
    {
        GroupsManager::groupIntoCollage(m_coreAccessor->getExifReaderFactory(), *m_currentPrj.get(), photos);
    }
    else if (chosenAction == manageGroup)
    {
        assert(photos.size() == 1);
        const std::vector<Photo::Data> groupMembers = evaluate<std::vector<Photo::Data>(Database::IBackend &)>(db, [&photos](Database::IBackend& backend)
        {
            std::vector<Photo::Data> members;

            auto& groupOperator = backend.groupOperator();
            const auto memberIds = groupOperator.membersOf(photos.front().groupInfo.group_id);

            for (const auto& id: memberIds)
            {
                const Photo::Data member = backend.getPhoto(id);
                members.push_back(member);
            }

            return members;
        });

        IExifReaderFactory& factory = m_coreAccessor->getExifReaderFactory();
        auto logger = m_loggerFactory.get("PhotosGrouping");
        PhotosGroupingDialog dialog(groupMembers, factory, m_executor, m_configuration, logger.get());
        const int status = dialog.exec();

        if (status == QDialog::Accepted)
        {
            // remove old group
            removeGroupOf(photos);

            // create new one
            const QString representantPath = GroupsManager::copyRepresentatToDatabase(dialog.getRepresentative(), *m_currentPrj.get());
            GroupsManager::group(db, groupMembers, representantPath, dialog.groupType());

        }

    }
    else if (chosenAction == ungroupPhotos)
    {
       removeGroupOf(photos);
    }
    else if (chosenAction == location)
    {
        const Photo::Data& first = photos.front();
        const QString relative_path = first.path;
        const QString absolute_path = m_currentPrj->makePathAbsolute(relative_path);
        const QFileInfo photoFileInfo(absolute_path);
        const QString file_dir = photoFileInfo.path();

        QDesktopServices::openUrl(QUrl::fromLocalFile(file_dir));
    }
    else if (chosenAction == faces)
    {
        const Photo::Data& first = photos.front();
        const QString relative_path = first.path;
        const ProjectInfo prjInfo = m_currentPrj->getProjectInfo();

        FacesDialog faces_dialog(first, &m_completerFactory, m_coreAccessor, m_currentPrj.get());
        faces_dialog.exec();
    }
}


void MainWindow::removeGroupOf(const std::vector<Photo::Data>& representatives)
{
    for (const Photo::Data& representative: representatives)
    {
        const GroupInfo& grpInfo = representative.groupInfo;
        const Group::Id gid = grpInfo.group_id;

        assert(gid.valid());

        Database::IDatabase& db = m_currentPrj->getDatabase();
        GroupsManager::ungroup(db, gid);

        // delete representative file
        QFile::remove(representative.path);
    }
}


int MainWindow::reportWarning(const QString& warning)
{
    return m_notifications.insertWarning(warning);
}


void MainWindow::removeWarning(int id)
{
    m_notifications.removeWarningWithId(id);
}


void MainWindow::on_actionNew_collection_triggered()
{
    ProjectCreator prjCreator;
    const bool creation_status = prjCreator.create(m_prjManager, m_pluginLoader);

    if (creation_status)
        openProject(prjCreator.project(), true);
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

    QMessageBox::about(this, tr("About Photo Broom"), about);
}


void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}


void MainWindow::on_actionTags_editor_triggered()
{
    const bool state = ui->actionTags_editor->isChecked();

    ui->tagEditorDockWidget->setVisible(state);
}


void MainWindow::on_actionTasks_triggered()
{
    const bool state = ui->actionTasks->isChecked();

    ui->tasksDockWidget->setVisible(state);
}


void MainWindow::on_actionPhoto_properties_triggered()
{
    const bool state = ui->actionPhoto_properties->isChecked();

    ui->photoPropertiesDockWidget->setVisible(state);
}


void MainWindow::on_actionSeries_detector_triggered()
{
    SeriesDetection{m_currentPrj->getDatabase(), m_coreAccessor, m_thumbnailsManager, *m_currentPrj.get()}.exec();
}

void MainWindow::on_actionPhoto_data_completion_triggered()
{
    QObject* mainwindow = QmlUtils::findQmlObject(ui->mainViewQml, "MainWindow");
    mainwindow->setProperty("currentIndex", 1);
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

            // TODO: I do not like this flag here...
            if (is_new)
                on_actionScan_collection_triggered();
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

        case Database::StatusCodes::VersionTooOld:
            QMessageBox::critical(this,
                                  tr("Unsupported photo collection version"),
                                  tr("Photo collection you are trying to open uses database in version which is not supported.\n"
                                     "It means your database is too old to open it.\n\n")
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

        case Database::StatusCodes::ProjectLocked:
            QMessageBox::critical(this,
                                  tr("Collection locked"),
                                  tr("Photo collection could not be opened.\n"
                                     "It is already opened by another Photo Broom instance.")
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
