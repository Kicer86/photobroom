
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QQmlApplicationEngine>

#include <core/ifeatures_manager.hpp>
#include <database/idatabase.hpp>
#include <updater/iupdater.hpp>

#include "models/notifications_model.hpp"
#include "models/series_model.hpp"
#include "models/tasks_model.hpp"
#include "ui_utils/completer_factory.hpp"
#include "utils/features_observer.hpp"


class PhotosAnalyzer;
struct ICoreFactoryAccessor;
struct ILoggerFactory;
struct ITaskExecutor;
struct IPluginLoader;
class IProjectManager;
struct IConfiguration;
struct IView;

class Project;
struct ProjectInfo;
struct IThumbnailsManager;


class MainWindow: public QObject
{
        Q_OBJECT

    public:
        explicit MainWindow(IFeaturesManager &, ICoreFactoryAccessor *, IThumbnailsManager *);
        MainWindow(const MainWindow &) = delete;
        virtual ~MainWindow();

        MainWindow operator=(const MainWindow &) = delete;

        void set(IProjectManager *);
        void set(IPluginLoader *);
        void set(IUpdater *);

    private:
        TasksModel                m_tasksModel;
        std::unique_ptr<ILogger>  m_logger;
        IProjectManager*          m_prjManager;
        IPluginLoader*            m_pluginLoader;
        std::unique_ptr<Project>  m_currentPrj;
        IConfiguration&           m_configuration;
        IUpdater*                 m_updater;
        ICoreFactoryAccessor*     m_coreAccessor;
        IThumbnailsManager*       m_thumbnailsManager;
        QPointer<QObject>         m_collectionScanner;
        QQmlApplicationEngine     m_mainView;
        std::unique_ptr<PhotosAnalyzer> m_photosAnalyzer;
        CompleterFactory          m_completerFactory;
        NotificationsModel        m_notifications;
        FeaturesObserver          m_featuresObserver;

        Q_INVOKABLE void openProject(const QString &, bool = false);
        void closeProject();
        void updateGui();
        void updateTools();
        void updateProjectProperties();
        void loadRecentCollections();

        void setupQmlView();
        void setupConfig();

    private slots:
        // album menu
        void on_actionNew_collection_triggered();
        void on_actionOpen_collection_triggered();
        void on_actionClose_triggered();

        // photos menu
        void on_actionScan_collection_triggered();

        // help menu
        void on_actionHelp_triggered();
        void on_actionAbout_triggered();
        void on_actionAbout_Qt_triggered();

        //internal slots
        void projectOpened(bool);
        void showProjectOpeningError(const QString& prjPath, const Database::BackendStatus &);

        //check version
        void checkVersion();

        //
        void currentVersion(const IUpdater::OnlineVersion &);

    signals:
        void currentDatabaseChanged(Database::IDatabase *);          // emit when database is opened/closed
        void currentProjectChanged(Project *);
};

#endif // MAINWINDOW_HPP
