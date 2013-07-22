
#include "mainwindow.hpp"

#include "centralwidget.hpp"

namespace
{



}


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
