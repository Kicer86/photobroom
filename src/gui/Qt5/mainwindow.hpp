
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>

class IProject;
class IProjectManager;

class CentralWidget;

class MainWindow final: public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        MainWindow(const MainWindow &) = delete;
        virtual ~MainWindow();

        MainWindow operator=(const MainWindow &) = delete;

        void set(IProjectManager *);

    private:
        IProjectManager*          m_prjManager;
        std::shared_ptr<IProject> m_currentPrj;
        CentralWidget*            m_centralWidget;

        void closeEvent(QCloseEvent *);

    private slots:
        void newProject();
        void openProject();
};

#endif // MAINWINDOW_HPP
