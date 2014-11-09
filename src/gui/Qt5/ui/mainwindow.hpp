
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>

class StagedPhotosDataModel;
class PhotosCollector;
struct ITaskExecutor;
struct IPluginLoader;
struct IProject;
struct IProjectManager;
struct IConfiguration;
struct IView;

class DBDataModel;

namespace Ui
{
    class MainWindow;
}

class MainWindow final: public QMainWindow
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

    private:
        Ui::MainWindow*           ui;
        IProjectManager*          m_prjManager;
        IPluginLoader*            m_pluginLoader;
        std::shared_ptr<IProject> m_currentPrj;
        DBDataModel*              m_imagesModel;
        StagedPhotosDataModel*    m_stagedImagesModel;
        IConfiguration*           m_configuration;
        PhotosCollector*          m_photosCollector;
        std::vector<IView *>      m_views;

        void closeEvent(QCloseEvent *);
        void openProject(const QString &);
        void setupView();
        void createMenus();
        void updateMenus();
        void viewChanged();

    private slots:
        void on_actionNew_project_triggered();
        void on_actionOpen_project_triggered();
        void on_actionAdd_photos_triggered();
        void activateWindow(QAction *);
};

#endif // MAINWINDOW_HPP
