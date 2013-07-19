
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


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), m_gui(new GuiData(this))
{

}


MainWindow::~MainWindow()
{
}
