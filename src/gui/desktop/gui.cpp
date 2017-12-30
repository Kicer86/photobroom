
#include "gui.hpp"

#include <QApplication>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTranslator>

#include <core/constants.hpp>
#include <core/iconfiguration.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/ilogger.hpp>
#include <core/itask_executor.hpp>
#include <core/ilogger_factory.hpp>
#include <system/filesystem.hpp>

#ifdef UPDATER_ENABLED
#include <updater/updater.hpp>
#endif

#include "ui/mainwindow.hpp"


Gui::Gui(int& argc, char **argv):
    m_app(new QApplication(argc, argv)),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr),
    m_coreFactory(nullptr)
{

}


Gui::~Gui()
{

}


QCoreApplication* Gui::getApp()
{
    return m_app.get();
}


void Gui::set(IProjectManager* prjManager)
{
    m_prjManager = prjManager;
}


void Gui::set(IPluginLoader* pluginLoader)
{
    m_pluginLoader = pluginLoader;
}


void Gui::set( ICoreFactoryAccessor* coreFactory)
{
    m_coreFactory = coreFactory;
}


void Gui::run()
{
    assert(m_prjManager != nullptr);
    assert(m_pluginLoader != nullptr);
    assert(m_coreFactory != nullptr);

#ifdef GUI_STATIC
    // see: http://doc.qt.io/qt-5/resources.html
    Q_INIT_RESOURCE(images);
#endif

    // On Windows, add extra location for Qt plugins
#ifdef OS_WIN
    m_app->addLibraryPath(FileSystem().getLibrariesPath());
#endif

    ILoggerFactory* loggerFactory = m_coreFactory->getLoggerFactory();

    auto gui_logger = loggerFactory->get("Gui");
    auto photos_manager_logger = loggerFactory->get("Photos manager");

    const QString tr_path = FileSystem().getTranslationsPath();
    InfoStream( gui_logger.get()) << QString("Searching for translations in: %1").arg(tr_path);

    // translations
    QTranslator translator;
    translator.load("photo_broom_pl", tr_path);
    const bool status = QCoreApplication::installTranslator(&translator);

    if (status)
        gui_logger->log(ILogger::Severity::Info, "Polish translations loaded successfully.");
    else
        gui_logger->log(ILogger::Severity::Error, "Could not load Polish translations.");

    // setup basic configuration
    IConfiguration* configuration = m_coreFactory->getConfiguration();
    configuration->setDefaultValue(ExternalToolsConfigKeys::aisPath, QStandardPaths::findExecutable("align_image_stack"));
    configuration->setDefaultValue(ExternalToolsConfigKeys::convertPath, QStandardPaths::findExecutable("convert"));
    configuration->setDefaultValue(ExternalToolsConfigKeys::ffmpegPath, QStandardPaths::findExecutable("ffmpeg"));

    const QVariant ffmpegPath = configuration->getEntry(ExternalToolsConfigKeys::ffmpegPath);
    const QFileInfo fileInfo(ffmpegPath.toString());

    if (fileInfo.isExecutable() == false)
        gui_logger->warning("Path to FFMpeg tool is invalid. Thumbnails for video files will not be available.");

    // main window
    MainWindow mainWindow(m_coreFactory);

    mainWindow.set(m_prjManager);
    mainWindow.set(m_pluginLoader);

    // updater
#ifdef UPDATER_ENABLED
    Updater updater;
    mainWindow.set(&updater);
#endif

    mainWindow.show();
    m_app->exec();

    //stop all tasks
    ITaskExecutor* taskExecutor = m_coreFactory->getTaskExecutor();
    taskExecutor->stop();
}
