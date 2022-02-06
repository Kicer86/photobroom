
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>

#include <core/ifeatures_manager.hpp>
#include <database/idatabase.hpp>
#include <updater/iupdater.hpp>

#include "models/notifications_model.hpp"
#include "ui_utils/completer_factory.hpp"
#include "utils/features_observer.hpp"

class ConfigDialogManager;
class LookTabController;
class MainTabController;
class ToolsTabController;
class PhotosAnalyzer;
class PhotosWidget;
struct ICoreFactoryAccessor;
struct ILoggerFactory;
struct ITaskExecutor;
struct IPluginLoader;
struct IProjectManager;
struct IConfiguration;
struct IView;

class Project;
struct ProjectInfo;
struct IThumbnailsManager;

namespace Ui
{
    class MainWindow;
}

class MainWindow: public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(IFeaturesManager &, ICoreFactoryAccessor *, IThumbnailsManager *, QWidget *parent = 0);
        MainWindow(const MainWindow &) = delete;
        virtual ~MainWindow();

        MainWindow operator=(const MainWindow &) = delete;

        void set(IProjectManager *);
        void set(IPluginLoader *);
        void set(IUpdater *);

    private:
        Ui::MainWindow*           ui;
        IProjectManager*          m_prjManager;
        IPluginLoader*            m_pluginLoader;
        std::unique_ptr<Project>  m_currentPrj;
        IConfiguration&           m_configuration;
        ILoggerFactory&           m_loggerFactory;
        IUpdater*                 m_updater;
        ICoreFactoryAccessor*     m_coreAccessor;
        IThumbnailsManager*       m_thumbnailsManager;
        std::unique_ptr<PhotosAnalyzer> m_photosAnalyzer;
        std::unique_ptr<ConfigDialogManager> m_configDialogManager;
        std::unique_ptr<MainTabController> m_mainTabCtrl;
        std::unique_ptr<ToolsTabController> m_toolsTabCtrl;
        QStringList               m_recentCollections;
        CompleterFactory          m_completerFactory;
        NotificationsModel        m_notifications;
        FeaturesObserver          m_featuresObserver;

        void closeEvent(QCloseEvent *) override;

        void openProject(const ProjectInfo &, bool = false);
        void closeProject();
        void setupView();
        void updateMenus();
        void updateTitle();
        void updateGui();
        void updateTools();
        void updateWidgets();
        void registerConfigTab();

        void loadGeometry();
        void loadRecentCollections();

        void setupQmlView();
        void setupConfig();

    private slots:
        // album menu
        void on_actionNew_collection_triggered();
        void on_actionOpen_collection_triggered();
        void on_actionClose_triggered();
        void on_actionQuit_triggered();

        // photos menu
        void on_actionScan_collection_triggered();

        // help menu
        void on_actionHelp_triggered();
        void on_actionAbout_triggered();
        void on_actionAbout_Qt_triggered();

        // windows menu
        void on_actionTags_editor_triggered();
        void on_actionTasks_triggered();
        void on_actionPhoto_properties_triggered();

        // tools menu
        void on_actionSeries_detector_triggered();
        void on_actionPhoto_data_completion_triggered();

        // settings menu
        void on_actionConfiguration_triggered();

        //internal slots
        void projectOpened(const Database::BackendStatus &, bool);

        //check version
        void checkVersion();

        // update windows menu
        void updateWindowsMenu();

        //
        void currentVersion(const IUpdater::OnlineVersion &);

    signals:
        void currentDatabaseChanged(Database::IDatabase *);          // emit when database is opened/closed
        void currentProjectChanged(Project *);
};

#endif // MAINWINDOW_HPP
