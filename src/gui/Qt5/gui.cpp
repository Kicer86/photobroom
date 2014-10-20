
#include "gui.hpp"

#include <QApplication>
#include <QTranslator>

#include <core/ilogger.hpp>
#include <system/filesystem.hpp>

#include "ui/mainwindow.hpp"


Gui::Gui(): m_prjManager(nullptr), m_pluginLoader(nullptr), m_taskExecutor(nullptr), m_configuration(nullptr), m_logger(nullptr)
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


void Gui::set(ILogger* logger)
{
    m_logger = logger;
}


void Gui::run(int argc, char **argv)
{
    QApplication app(argc, argv);

    QTranslator translator;
    translator.load("photo_broom_pl", FileSystem::getTranslationsPath());
    const bool status = app.installTranslator(&translator);

    if (status)
        m_logger->log("gui", ILogger::Severity::Info, "Polish translations loaded successfully.");
    else
        m_logger->log("gui", ILogger::Severity::Error, "Could not load Polish translations.");

    MainWindow mainWindow;

    mainWindow.set(m_prjManager);
    mainWindow.set(m_pluginLoader);
    mainWindow.set(m_taskExecutor);
    mainWindow.set(m_configuration);

    mainWindow.show();
    app.exec();
}
