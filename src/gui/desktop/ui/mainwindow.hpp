
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>

#include <database/idatabase.hpp>
#include <updater/iupdater.hpp>

#include "ui_utils/completer_factory.hpp"
#include "utils/selection_extractor.hpp"

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

class DBDataModel;
class Project;
struct ProjectInfo;

namespace Ui
{
    class MainWindow;
}

class MainWindow: public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow( ICoreFactoryAccessor *, QWidget *parent = 0);
        MainWindow(const MainWindow &) = delete;
        virtual ~MainWindow();

        MainWindow operator=(const MainWindow &) = delete;

        void set(IProjectManager *);
        void set(IPluginLoader *);
        void set(IUpdater *);

    private:
        SelectionExtractor        m_selectionExtractor;
        Ui::MainWindow*           ui;
        IProjectManager*          m_prjManager;
        IPluginLoader*            m_pluginLoader;
        std::unique_ptr<Project>  m_currentPrj;
        DBDataModel*              m_imagesModel;
        DBDataModel*              m_newImagesModel;
        IConfiguration*           m_configuration;
        ILoggerFactory*           m_loggerFactory;
        IUpdater*                 m_updater;
        ITaskExecutor*            m_executor;
        std::unique_ptr<PhotosAnalyzer> m_photosAnalyzer;
        std::unique_ptr<ConfigDialogManager> m_configDialogManager;
        std::unique_ptr<MainTabController> m_mainTabCtrl;
        std::unique_ptr<LookTabController> m_lookTabCtrl;
        std::unique_ptr<ToolsTabController> m_toolsTabCtrl;
        QStringList               m_recentCollections;
        CompleterFactory          m_completerFactory;

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

        void setupReviewedPhotosView();
        void setupNewPhotosView();
        void setupConfig();

        void markPhotosReviewed(const IPhotoInfo::List &);
        void showContextMenuFor(PhotosWidget *, const QPoint &);

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
        void markNewPhotosAsReviewed();
        void viewChanged(int);
};

#endif // MAINWINDOW_HPP
