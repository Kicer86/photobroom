
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>


class MainWindow final: public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        virtual ~MainWindow();

        MainWindow operator=(const MainWindow &) = delete;

    private:
        void closeEvent(QCloseEvent *);

    private slots:
        void newProject();
        void openProject();
};

#endif // MAINWINDOW_HPP
