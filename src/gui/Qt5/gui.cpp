
#include "gui.hpp"

#include <QApplication>

#include "ui/mainwindow.hpp"


Gui::Gui(): m_prjManager(nullptr), m_pluginLoader(nullptr), m_taskExecutor(nullptr), m_configuration(nullptr)
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


void Gui::set(ITaskExecutor* taskExecutor)
{
    m_taskExecutor = taskExecutor;
}


void Gui::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}


void Gui::run(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow mainWindow;

    mainWindow.set(m_prjManager);
    mainWindow.set(m_pluginLoader);
    mainWindow.set(m_taskExecutor);
    mainWindow.set(m_configuration);

    mainWindow.show();
    app.exec();
}
