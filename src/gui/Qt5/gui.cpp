
#include "gui.hpp"

#include <QApplication>

#include "mainwindow.hpp"


Gui::Gui(): m_prjManager(nullptr)
{

}

void Gui::set(IProjectManager* prjManager)
{
    m_prjManager = prjManager;
}


void Gui::run(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow mainWindow;

    mainWindow.set(m_prjManager);

    mainWindow.show();
    app.exec();
}
