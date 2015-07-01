
#include "gui.hpp"

#include <QApplication>
#include <QTranslator>

#include <core/ilogger.hpp>
#include <core/itask_executor.hpp>
#include <core/ilogger_factory.hpp>
#include <system/filesystem.hpp>
#include <updater/updater.hpp>

#include "ui/mainwindow.hpp"


Gui::Gui(): m_prjManager(nullptr), m_pluginLoader(nullptr), m_taskExecutor(nullptr), m_configuration(nullptr), m_logger(nullptr)
{

}


Gui::~Gui()
{

}


std::unique_ptr<QCoreApplication> Gui::init(int argc, char** argv)
{
    return std::make_unique<QApplication>(argc, argv);
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


void Gui::set(ILoggerFactory* logger_factory)
{
    m_logger = logger_factory->get("gui");
}


void Gui::run()
{
#ifdef GUI_STATIC
    // see: http://doc.qt.io/qt-5/resources.html
    Q_INIT_RESOURCE(images);
#endif

    // On Windows, add extra location for Qt plugins
#ifdef OS_WIN
    QCoreApplication::addLibraryPath(FileSystem().getLibrariesPath());
#endif

	const QString tr_path = FileSystem().getTranslationsPath();
	m_logger->log(ILogger::Severity::Info, QString("Searching for translations in: %1").arg(tr_path).toStdString());

    QTranslator translator;
    translator.load("photo_broom_pl", tr_path);
    const bool status = QCoreApplication::installTranslator(&translator);

    if (status)
        m_logger->log(ILogger::Severity::Info, "Polish translations loaded successfully.");
    else
        m_logger->log(ILogger::Severity::Error, "Could not load Polish translations.");

    Updater updater;

    MainWindow mainWindow;

    mainWindow.set(m_prjManager);
    mainWindow.set(m_pluginLoader);
    mainWindow.set(m_taskExecutor);
    mainWindow.set(m_configuration);
    mainWindow.set(&updater);

    mainWindow.show();
    QCoreApplication::exec();

    //stop all tasks
    m_taskExecutor->stop();
}
