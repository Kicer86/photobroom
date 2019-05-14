
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <QTimer>

#ifdef OS_WIN
#include <Windows.h>
#endif

#include <core/configuration.hpp>
#include <core/core_factory_accessor.hpp>
#include <core/exif_reader_factory.hpp>
#include <core/features_manager.hpp>
#include <core/logger_factory.hpp>
#include <core/python_thread.hpp>
#include <core/task_executor.hpp>
#include <core/ilogger.hpp>
#include <crash_catcher/crash_catcher.hpp>
#include <database/database_builder.hpp>
#include <gui/gui.hpp>
#include <plugins/plugin_loader.hpp>
#include <project_utils/project_manager.hpp>
#include <system/system.hpp>

#include "paths.hpp"


enum class CrashCatcherStatus
{
    Disabled,
    Ok,
    Error,
};


int main(int argc, char **argv)
{
    Gui gui(argc, argv);

    QCoreApplication* app = gui.getApp();
    app->setApplicationName("photo_broom");                                // without this app name may change when binary name changes

    const QString basePath = System::getApplicationConfigDir();

    // setup internal locations
    QDir::setSearchPaths("icons", {QString(Paths::icons) + "/svg"});

    // perform command line parsing
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption logingLevelOption("loging-level",
                                         QCoreApplication::translate("main", "Defines loging level. Possible options are: Debug, Info, Warning (default), Error"),
                                         QCoreApplication::translate("main", "loging level"),
                                         "Warning"
                       );

    QCommandLineOption crashTestOption("test-crash-catcher", "When specified, photo_broom will crash 3 seconds after being launch");
    crashTestOption.setHidden(true);

    QCommandLineOption disableCrashCatcher("disable-crash-catcher", "Turns off crash catcher");

#ifdef OS_WIN
    QCommandLineOption enableConsole("enable-console", "Opens console with app's output messages");
    enableConsole.setHidden(true);

    parser.addOption(enableConsole);
#endif

    parser.addOption(logingLevelOption);
    parser.addOption(crashTestOption);
    parser.addOption(disableCrashCatcher);

    parser.process(*app);

    const bool enableCrashTest = parser.isSet(crashTestOption);
    if (enableCrashTest)
        QTimer::singleShot(3000, []
        {
            int* ptr = nullptr;
            volatile int v = *ptr;
            (void) v;
        });

    const QString logingLevelStr = parser.value(logingLevelOption);
    ILogger::Severity logingLevel = ILogger::Severity::Warning;

    if (logingLevelStr == "Debug")
        logingLevel = ILogger::Severity::Debug;
    else if (logingLevelStr == "Info")
        logingLevel = ILogger::Severity::Info;
    else if (logingLevelStr == "Warning")
        logingLevel = ILogger::Severity::Warning;
    else if (logingLevelStr == "Error")
        logingLevel = ILogger::Severity::Error;
    else
    {
        std::cerr << "Invalid option: '" << logingLevelStr.toStdString() << "' for loging-level option" << std::endl;
        return 1;
    }

    const bool crashCatcherDisabled = parser.isSet(disableCrashCatcher);

#ifdef OS_WIN
    if (parser.isSet(enableConsole) && (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) )
    {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
#endif

    // build objects
    FeaturesManager features_manager;

    CrashCatcherStatus status = CrashCatcherStatus::Disabled;

    if (crashCatcherDisabled == false)
        status = CrashCatcher::init(argv[0]) ? CrashCatcherStatus::Ok : CrashCatcherStatus::Error;

    LoggerFactory logger_factory(basePath);
    logger_factory.setLogingLevel(logingLevel);

    Configuration configuration;

    PythonThread pythonThread;

    PluginLoader pluginLoader;
    pluginLoader.set(&logger_factory);

    auto taskExecutorLogger = logger_factory.get("TaskExecutor");
    TaskExecutor taskExecutor(taskExecutorLogger.get());

    Database::Builder database_builder;
    database_builder.set(&pluginLoader);
    database_builder.set(&logger_factory);
    database_builder.set(&configuration);

    ProjectManager prjManager(&database_builder);

    switch (status)
    {
        case CrashCatcherStatus::Ok:
            logger_factory.get("CrashCatcher")->info("Initialization successful");
            break;

        case CrashCatcherStatus::Error:
            logger_factory.get("CrashCatcher")->error("Initialization failed");
            break;

        case CrashCatcherStatus::Disabled:
            logger_factory.get("CrashCatcher")->info("Disabled");
            break;
    }

    ExifReaderFactory exifReaderFactory;

    CoreFactoryAccessor coreFactory(&logger_factory,
                                    &exifReaderFactory,
                                    &configuration,
                                    &taskExecutor,
                                    &pythonThread
    );

    // start gui
    gui.set(&prjManager);
    gui.set(&pluginLoader);
    gui.set(&coreFactory);
    gui.run();

    taskExecutor.stop();

    System::cleanTemporaries();

    return 0;
}
