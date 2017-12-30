
#include "mainwindow.hpp"

#include <functional>

#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>

#include <core/constants.hpp>
#include <core/cross_thread_call.hpp>
#include <core/exif_reader_factory.hpp>
#include <core/iconfiguration.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include <database/database_builder.hpp>
#include <database/idatabase.hpp>
#include <database/database_tools/photos_analyzer.hpp>
#include <project_utils/iproject_manager.hpp>
#include <project_utils/misc.hpp>
#include <project_utils/project.hpp>

#include "config.hpp"

#include "config_keys.hpp"
#include "config_tabs/look_tab.hpp"
#include "config_tabs/main_tab.hpp"
#include "config_tabs/tools_tab.hpp"
#include "models/db_data_model.hpp"
#include "widgets/info_widget.hpp"
#include "widgets/project_creator/project_creator_dialog.hpp"
#include "widgets/photos_widget.hpp"
#include "widgets/collection_dir_scan_dialog.hpp"
#include "ui_utils/config_dialog_manager.hpp"
#include "utils/photos_collector.hpp"
#include "ui_utils/icons_loader.hpp"
#include "ui_mainwindow.h"
#include "ui/photos_grouping_dialog.hpp"


namespace
{

    struct StagePhotosTask final: ITaskExecutor::ITask
    {
        StagePhotosTask(const IPhotoInfo::List& photos): m_photos(photos) {}

        std::string name() const override
        {
            return "Store photos";
        }

        void perform() override
        {
            for(const IPhotoInfo::Ptr& photo: m_photos)
                photo->markFlag(Photo::FlagsE::StagingArea, 0);
        }

        private:
            IPhotoInfo::List m_photos;
    };

}


MainWindow::MainWindow( ICoreFactoryAccessor* coreFactory, QWidget *p): QMainWindow(p),
    m_selectionExtractor(),
    ui(new Ui::MainWindow),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr),
    m_currentPrj(nullptr),
    m_imagesModel(nullptr),
    m_newImagesModel(nullptr),
    m_configuration(coreFactory->getConfiguration()),
    m_loggerFactory(coreFactory->getLoggerFactory()),
    m_updater(nullptr),
    m_executor(coreFactory->getTaskExecutor()),
    m_photosAnalyzer(new PhotosAnalyzer(coreFactory)),
    m_configDialogManager(new ConfigDialogManager),
    m_mainTabCtrl(new MainTabController ),
    m_lookTabCtrl(new LookTabController ),
    m_toolsTabCtrl(new ToolsTabController ),
    m_recentCollections(),
    m_completerFactory()
{
    // setup
    ui->setupUi(this);
    setupConfig();
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

    ui->photoPropertiesWidget->set(&m_selectionExtractor);

    // post construction initialization
    ui->imagesView->set(m_executor);
    ui->newImagesView->set(m_executor);
    m_imagesModel->set(m_executor);

    m_mainTabCtrl->set(m_configuration);
    m_lookTabCtrl->set(m_configuration);
    m_toolsTabCtrl->set(m_configuration);
    ui->imagesView->set(m_configuration);
    ui->newImagesView->set(m_configuration);

    m_completerFactory.set(m_loggerFactory);

    // TODO: Not nice to have setters for views here :/
    // Views will use completer factories immediately after set.
    // So factories need log factory before it.
    ui->imagesView->set(&m_completerFactory);
    ui->imagesView->set(m_loggerFactory);
    ui->newImagesView->set(&m_completerFactory);
    ui->newImagesView->set(m_loggerFactory);
    ui->tagEditor->set(&m_completerFactory);
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


void MainWindow::setupConfig()
{
    // setup defaults
    m_configuration->setDefaultValue(UpdateConfigKeys::updateEnabled,   true);

    m_configuration->setDefaultValue(ViewConfigKeys::itemsMargin,    10);
    m_configuration->setDefaultValue(ViewConfigKeys::itemsSpacing,   2);
    m_configuration->setDefaultValue(ViewConfigKeys::thumbnailWidth, 120);
    m_configuration->setDefaultValue(ViewConfigKeys::bkg_color_even, 0xff000040u);
    m_configuration->setDefaultValue(ViewConfigKeys::bkg_color_odd,  0x0000ff40u);

    loadGeometry();
    loadRecentCollections();
}


void MainWindow::set(IUpdater* updater)
{
    m_updater = updater;

    const bool enabled = m_configuration->getEntry(UpdateConfigKeys::updateEnabled).toBool();

    if (enabled)
    {
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        const QVariant last_raw = m_configuration->getEntry(UpdateConfigKeys::lastCheck);
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
            m_configuration->setEntry(UpdateConfigKeys::lastCheck, now_duration_raw);
        }
    }
}


void MainWindow::checkVersion()
{
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

    //store recent collections
    m_configuration->setEntry("gui::recent", m_recentCollections.join(";"));
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

        m_imagesModel->setDatabase(nullptr);
        m_newImagesModel->setDatabase(nullptr);
        QDir::setSearchPaths("prj", QStringList() );

        updateGui();
    }
}


void MainWindow::setupView()
{
    m_imagesModel = new DBDataModel(this);
    ui->imagesView->setModel(m_imagesModel);

    m_newImagesModel = new DBDataModel(this);
    ui->newImagesView->setModel(m_newImagesModel);

    setupReviewedPhotosView();
    setupNewPhotosView();

    m_photosAnalyzer->set(ui->tasksWidget);

    // connect to docks
    connect(ui->tagEditorDockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(updateWindowsMenu()));
    connect(ui->tasksDockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(updateWindowsMenu()));
    connect(ui->photoPropertiesDockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(updateWindowsMenu()));

    // connect to tabs
    connect(ui->viewsStack, &QTabWidget::currentChanged, this, &MainWindow::viewChanged);

    // trigger initial setup of tags editor and photo properties dock
    viewChanged(ui->viewsStack->currentIndex());

    // connect to context menu for views
    connect(ui->imagesView, &QWidget::customContextMenuRequested, [this](const QPoint& p) { this->showContextMenuFor(ui->imagesView, p); });
    connect(ui->newImagesView, &QWidget::customContextMenuRequested, [this](const QPoint& p) { this->showContextMenuFor(ui->newImagesView, p); });
}


void MainWindow::updateMenus()
{
    const bool prj = m_currentPrj.get() != nullptr;
    const bool valid = m_recentCollections.isEmpty() == false;

    ui->menuPhotos->menuAction()->setVisible(prj);
    ui->menuOpen_recent->menuAction()->setVisible(valid);
    ui->menuOpen_recent->clear();

    for(const QString& entry: m_recentCollections)
    {
        QAction* action = ui->menuOpen_recent->addAction(entry);
        connect(action, &QAction::triggered, [=]
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
        m_photosAnalyzer->setDatabase(m_currentPrj->getDatabase());
    else
        m_photosAnalyzer->setDatabase(nullptr);
}


void MainWindow::updateWidgets()
{
    const bool prj = m_currentPrj.get() != nullptr;

    ui->viewsStack->setEnabled(prj);
    ui->tagEditor->setEnabled(prj);
}


void MainWindow::registerConfigTab()
{
    m_configDialogManager->registerTab(m_mainTabCtrl.get());
    m_configDialogManager->registerTab(m_lookTabCtrl.get());
    m_configDialogManager->registerTab(m_toolsTabCtrl.get());
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


void MainWindow::loadRecentCollections()
{
    // recent collections
    const QString rawList = m_configuration->getEntry("gui::recent").toString();

    if (rawList.isEmpty() == false)
        m_recentCollections = rawList.split(";");

    updateMenus();
}


void MainWindow::setupReviewedPhotosView()
{
    auto reviewed_photos_filter = std::make_shared<Database::FilterPhotosWithFlags>();
    auto group_members_filter = std::make_shared<Database::FilterPhotosWithRole>(Database::FilterPhotosWithRole::Role::GroupMember);
    auto not_group_members_filter = std::make_shared<Database::FilterNotMatchingFilter>(group_members_filter);

    reviewed_photos_filter->flags[Photo::FlagsE::StagingArea] = 0;

    std::deque<Database::IFilter::Ptr> reviewedPhotosFilters = {reviewed_photos_filter, not_group_members_filter};

    m_imagesModel->setStaticFilters(reviewedPhotosFilters);
    ui->imagesView->setBottomHintWidget(nullptr);
}


void MainWindow::setupNewPhotosView()
{
    auto new_photos_filter = std::make_shared<Database::FilterPhotosWithFlags>();
    auto group_members_filter = std::make_shared<Database::FilterPhotosWithRole>(Database::FilterPhotosWithRole::Role::GroupMember);
    auto not_group_members_filter = std::make_shared<Database::FilterNotMatchingFilter>(group_members_filter);

    new_photos_filter->flags[Photo::FlagsE::StagingArea] = 1;

    std::deque<Database::IFilter::Ptr> newPhotosFilters = {new_photos_filter, not_group_members_filter};

    m_newImagesModel->setStaticFilters(newPhotosFilters);

    InfoBalloonWidget* hint = new InfoBalloonWidget (ui->imagesView);
    const QString message = tr("Above you can view new photos and describe them.");
    const QString link = tr("You can click here when you are done to mark photos as reviewed.");
    hint->setText( QString("%1<br/><a href=\"reviewed\">%2</a>").arg(message).arg(link) );
    hint->setTextFormat(Qt::RichText);
    ui->newImagesView->setBottomHintWidget(hint);

    connect(hint, &QLabel::linkActivated, this, &MainWindow::markNewPhotosAsReviewed);
}


void MainWindow::markPhotosReviewed(const IPhotoInfo::List& photos)
{
    // add task for photos modification, so main thread will not be slowed down
    auto task = std::make_unique<StagePhotosTask>(photos);
    m_executor->add(std::move(task));
}


void MainWindow::showContextMenuFor(PhotosWidget* photosView, const QPoint& pos)
{
    const std::vector<IPhotoInfo::Ptr> selected_photos = m_selectionExtractor.getSelection();

    std::vector<IPhotoInfo::Ptr> photos;
    std::remove_copy_if(selected_photos.cbegin(),
                        selected_photos.cend(),
                        std::back_inserter(photos),
                        [](const IPhotoInfo::Ptr& photo){
                            return QFile::exists(photo->getPath()) == false;
                        });

    QMenu contextMenu;
    QAction* groupPhotos = contextMenu.addAction(tr("Group"));
    QAction* location    = contextMenu.addAction(tr("Open photo location"));

    groupPhotos->setEnabled(photos.size() > 1);
    location->setEnabled(photos.size() > 0);

    const QPoint globalPos = photosView->mapToGlobal(pos);
    QAction* chosenAction = contextMenu.exec(globalPos);

    if (chosenAction == groupPhotos)
    {
        ExifReaderFactory factory;

        IExifReader* reader = factory.get();

        auto logger = m_loggerFactory->get("PhotosGrouping");

        PhotosGroupingDialog dialog(photos, reader, m_executor, m_configuration, logger.get());
        const int status = dialog.exec();

        if (status == QDialog::Accepted)
        {
            const QString photo = dialog.getRepresentative();

            std::vector<Photo::Id> photos_ids;
            for(std::size_t i = 0; i < photos.size(); i++)
                photos_ids.push_back(photos[i]->getID());

            const QString internalPath = copyFileToPrivateMediaLocation(m_currentPrj->getProjectInfo(), photo);
            const QString internalPathDecorated = m_currentPrj->makePathRelative(internalPath);

            DBDataModel* model = photosView->getModel();
            model->group(photos_ids, internalPathDecorated);
        }
    }
    else if (chosenAction == location)
    {
        if (photos.empty() == false)
        {
            const IPhotoInfo::Ptr& first = photos.front();
            const QString relative_path = first->getPath();
            const QString absolute_path = m_currentPrj->makePathAbsolute(relative_path);
            const QFileInfo photoFileInfo(absolute_path);
            const QString file_dir = photoFileInfo.path();

            QDesktopServices::openUrl(QUrl::fromLocalFile(file_dir));
        }
    }
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
    Database::IDatabase* db = m_currentPrj->getDatabase();

    CollectionDirScanDialog scanner(m_currentPrj.get(), db);
    const int status = scanner.exec();

    if (status == QDialog::Accepted)
    {
        const std::set<QString>& paths = scanner.newPhotos();

        std::deque<Photo::Data> photos;
        for(const QString& path: paths)
        {
            Photo::Data photo_data;
            photo_data.path = path;
            photo_data.flags = { {Photo::FlagsE::StagingArea, 1} };
            photos.emplace_back(photo_data);
        }

        db->store(photos);
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
            Database::IDatabase* db = m_currentPrj->getDatabase();

            m_imagesModel->setDatabase(db);
            m_newImagesModel->setDatabase(db);
            m_completerFactory.set(db);

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


void MainWindow::markNewPhotosAsReviewed()
{
    // TODO: use batch operations here (IDatabase::perform)
    // Current implementation is buggy and cannot be used.
    // Check commit 722821802d2af576f0d97fc4bc5a898033a87970
    // and issue #203
    using namespace std::placeholders;
    auto markPhotos = std::bind(&MainWindow::markPhotosReviewed, this, _1);
    auto filter = std::make_shared<Database::FilterPhotosWithFlags>();

    filter->flags[Photo::FlagsE::StagingArea] = 1;

    const std::deque<Database::IFilter::Ptr> filters( {filter});

    m_currentPrj->getDatabase()->listPhotos(filters, markPhotos);
}


void MainWindow::viewChanged(int current)
{
    QItemSelectionModel* selectionModel = nullptr;
    DBDataModel* dataModel = nullptr;
    //setup tags editor

    switch(current)
    {
        case 0:
            selectionModel = ui->imagesView->viewSelectionModel();
            dataModel = m_imagesModel;
            break;

        case 1:
            selectionModel = ui->newImagesView->viewSelectionModel();
            dataModel = m_newImagesModel;
            break;

        default:
            assert(!"Unexpected tab index");
            break;
    }

    ui->tagEditor->set(selectionModel);
    ui->tagEditor->set(dataModel);

    m_selectionExtractor.set(selectionModel);
    m_selectionExtractor.set(dataModel);
}
