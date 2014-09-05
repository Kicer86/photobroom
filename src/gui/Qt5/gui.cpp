
#include "gui.hpp"

#include <QApplication>

#include "mainwindow.hpp"


Gui::Gui(): m_prjManager(nullptr), m_pluginLoader(nullptr)
{

}

void Gui::set(IProjectManager* prjManager)
{
    m_prjManager = prjManager;
}


void Gui::set(IPluginLoader* pluginLoader)
{
    m_pluginLoader = pluginLoader;
}


void Gui::run(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow mainWindow;

    mainWindow.set(m_prjManager);
    mainWindow.set(m_pluginLoader);

    mainWindow.show();
    app.exec();
}
