
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>


class MainWindow final: public QMainWindow
{
    public:
        explicit MainWindow(QWidget *parent = 0);
        virtual ~MainWindow();

        MainWindow operator=(const MainWindow &) = delete;

    private:
        struct GuiData;
        std::unique_ptr<GuiData> m_gui;

        void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_HPP
