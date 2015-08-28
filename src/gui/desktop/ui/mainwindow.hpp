
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>

#include <database/idatabase.hpp>
#include <updater/iupdater.hpp>

class PhotosCollector;
class PhotosAnalyzer;
class ConfigDialogManager;
struct ITaskExecutor;
struct IPluginLoader;
struct IProject;
struct IProjectManager;
struct IConfiguration;
struct IView;

class DBDataModel;
struct ProjectInfo;

namespace Ui
{
    class MainWindow;
}

class MainWindow: public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        MainWindow(const MainWindow &) = delete;
        virtual ~MainWindow();

        MainWindow operator=(const MainWindow &) = delete;

        void set(IProjectManager *);
        void set(IPluginLoader *);
        void set(ITaskExecutor *);
        void set(IConfiguration *);
        void set(IUpdater *);

    private:
        Ui::MainWindow*           ui;
        IProjectManager*          m_prjManager;
        IPluginLoader*            m_pluginLoader;
        std::unique_ptr<IProject> m_currentPrj;
        DBDataModel*              m_imagesModel;
        IConfiguration*           m_configuration;
        IUpdater*                 m_updater;
        ITaskExecutor*            m_executor;
        std::unique_ptr<PhotosAnalyzer> m_photosAnalyzer;
        std::unique_ptr<ConfigDialogManager> m_configDialogManager;

        void closeEvent(QCloseEvent *) override;

        void openProject(const ProjectInfo &);
        void closeProject();
        void setupView();
        void updateMenus();
        void updateTitle();
        void updateGui();
        void updateTools();
        void updateWidgets();

        void loadGeometry();

    private slots:
        // album menu
        void on_actionNew_collection_triggered();
        void on_actionOpen_collection_triggered();
        void on_actionClose_triggered();
        void on_actionQuit_triggered();

        // photos menu
        void on_actionAdd_photos_triggered();

        // help menu
        void on_actionHelp_triggered();
        void on_actionAbout_triggered();
        void on_actionAbout_Qt_triggered();

        // settings menu
        void on_actionConfiguration_triggered();

        //internal slots
        void projectOpened(const Database::BackendStatus &);

        //check version
        void checkVersion();

        void currentVersion(const IUpdater::OnlineVersion &);
        void projectOpenedNotification(const Database::BackendStatus &);

    signals:
        void projectOpenedSignal(const Database::BackendStatus &);
};

#endif // MAINWINDOW_HPP
