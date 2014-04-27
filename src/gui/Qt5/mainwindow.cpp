
#include "mainwindow.hpp"

#include <QCloseEvent>

#include "database/databasebuilder.hpp"
#include "database/idatabase.hpp"

#include "centralwidget.hpp"


struct MainWindow::GuiData
{
    GuiData(QMainWindow *mainWindow)
    {
        mainWindow->setCentralWidget(new CentralWidget(mainWindow));
    }
};


MainWindow::MainWindow(QWidget *p): QMainWindow(p), m_gui(new GuiData(this))
{

}


MainWindow::~MainWindow()
{
}


void MainWindow::closeEvent(QCloseEvent *e)
{
    //Database::Builder::instance()->getBackend()->close();

    e->accept();
}
