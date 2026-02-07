
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <QImageReader>
#include <QTimer>

#ifdef OS_WIN
#include <Windows.h>
#endif

#include <core/core_factory_accessor.hpp>
#include <core/exif_reader_factory.hpp>
#include <core/ifeatures_manager.hpp>
#include <core/ilogger.hpp>
#include <core/logger_factory.hpp>
#include <core/task_executor.hpp>
#include <core/observable_task_executor.hpp>
#include <core/observables_registry.hpp>
#include <crash_catcher/crash_catcher.hpp>
#include <database/database_builder.hpp>
#include <gui/gui.hpp>
#include <plugins/plugin_loader.hpp>
#include <project_utils/project_manager.hpp>

#include "config.hpp"
#include "config_storage.hpp"
#include "features.hpp"
#include "paths.hpp"


import system;

namespace
{

    enum class CrashCatcherStatus
    {
        Disabled,
        Ok,
        Error,
    };

    class CommandLineToggles: public IFeaturesManager
    {
        public:
            CommandLineToggles(const std::set<QString>& toggles)
                : m_toggles(toggles)
            {

            }

            bool has(const QString& key) const override
            {
                return m_toggles.contains(key);
            }

        private:
            const std::set<QString> m_toggles;
    };
}


int main(int argc, char **argv)
{
#ifdef OS_WIN
    QCoreApplication::setLibraryPaths({ "qt_plugins" });
#endif

    QApplication app(argc, argv);

    app.setApplicationName("photo_broom");                   // without this app name may change when binary name changes
    app.setApplicationVersion(PHOTO_BROOM_VERSION);

    const QString basePath = System::getApplicationConfigDir();

    // setup internal locations
    QDir::setSearchPaths("icons", {QString(Paths::icons) + "/svg"});

    // perform command line parsing
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption loggingLevelOption("logging-level",
                                         QCoreApplication::translate("main", "Defines logging level. Possible options are: Trace, Debug, Info, Warning (default), Error"),
                                         QCoreApplication::translate("main", "logging level"),
                                         "Warning"
    );

    QCommandLineOption developerOptions("feature-toggle",
                                         QCoreApplication::translate("main", "Enables experimental features. Use for each flag you want to turn on: test-crash-catcher, debug-view"),
                                         QCoreApplication::translate("main", "flag")
    );

    QCommandLineOption disableCrashCatcher("disable-crash-catcher", "Turns off crash catcher");

    QCommandLineOption imageMemoryLimit("image-memory-limit",
                                        QCoreApplication::translate("main", "Limit for image sizes in megabytes. 512MB by default, which allows to open 512/4 = 128MPix photos."),
                                        QCoreApplication::translate("main", "image memory limit"),
                                        "512"
    );

    const int threadsCount = std::thread::hardware_concurrency();
    QCommandLineOption threadsLimit("threads-limit",
                                    QCoreApplication::translate("main", "Limit for working threads count. "
                                                                        "Photo broom will not use more working threads than specified here. "
                                                                        "Another limitation is number of available CPU cores which won't be exceeded either."),
                                    QCoreApplication::translate("main", "threads count"),
                                    QString::number(threadsCount)
    );

#ifdef OS_WIN
    QCommandLineOption enableConsole("enable-console", "Opens console with app's output messages");
    enableConsole.setFlags(QCommandLineOption::HiddenFromHelp);

    parser.addOption(enableConsole);
#endif

    parser.addOption(loggingLevelOption);
    parser.addOption(developerOptions);
    parser.addOption(disableCrashCatcher);
    parser.addOption(imageMemoryLimit);
    parser.addOption(threadsLimit);

    parser.process(app);

    const QStringList featureToggles = parser.values(developerOptions);
    if (featureToggles.contains("test-crash-catcher"))
    {
        QTimer::singleShot(3000, []
        {
            int* ptr = nullptr;
            volatile int v = *ptr;
            (void) v;
        });

        std::cout << "crash catcher test activated. Will crash in 3 seconds" << std::endl;
    }

    ObservablesRegistry::instance().enable(featureToggles.contains("debug-view"));

    const QString loggingLevelStr = parser.value(loggingLevelOption);
    ILogger::Severity loggingLevel = ILogger::Severity::Warning;

    if (loggingLevelStr == "Trace")
        loggingLevel = ILogger::Severity::Trace;
    else if (loggingLevelStr == "Debug")
        loggingLevel = ILogger::Severity::Debug;
    else if (loggingLevelStr == "Info")
        loggingLevel = ILogger::Severity::Info;
    else if (loggingLevelStr == "Warning")
        loggingLevel = ILogger::Severity::Warning;
    else if (loggingLevelStr == "Error")
        loggingLevel = ILogger::Severity::Error;
    else
    {
        std::cerr << "Invalid option: '" << loggingLevelStr.toStdString() << "' for logging-level option" << std::endl;
        return 1;
    }

    const int threadsLimitValue = parser.value(threadsLimit).toInt();
    const int threadsToUse = std::clamp(threadsLimitValue, 1, threadsCount);        // use at least 1 thread but no more than 'threadsCount' nor 'threadsLimitValue'
    const int memoryLimit = parser.value(imageMemoryLimit).toInt();
    const bool crashCatcherDisabled = parser.isSet(disableCrashCatcher);

#ifdef OS_WIN
    if (parser.isSet(enableConsole) && (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) )
    {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
#endif

    // build objects
    CrashCatcherStatus status = CrashCatcherStatus::Disabled;

    if (crashCatcherDisabled == false)
        status = CrashCatcher::init(argv[0]) ? CrashCatcherStatus::Ok : CrashCatcherStatus::Error;

    std::set<QString> toggles;
    CommandLineToggles cmdLineToggles(toggles);

    LoggerFactory logger_factory(basePath);
    logger_factory.setLoggingLevel(loggingLevel);

    const QString configFileDir = System::getApplicationConfigDir();
    const QString configFilePath = configFileDir + "/" + "config.json";
    ConfigStorage configStorage(configFilePath);
    Configuration configuration(configStorage);

    PluginLoader pluginLoader;
    pluginLoader.set(&logger_factory);

    auto taskExecutorLogger = logger_factory.get("TaskExecutor");
    ObservableTaskExecutor<TaskExecutor> taskExecutor("", *taskExecutorLogger, threadsToUse);

    Database::Builder database_builder;
    database_builder.set(&pluginLoader);
    database_builder.set(&logger_factory);
    database_builder.set(&configuration);

    ProjectManager prjManager(database_builder);

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

    CoreFactoryAccessor coreFactory(logger_factory,
                                    exifReaderFactory,
                                    configuration,
                                    taskExecutor
    );

    // setup
    QImageReader::setAllocationLimit(memoryLimit);

    // start gui
    Gui(prjManager, pluginLoader, coreFactory, cmdLineToggles).run();

    taskExecutor.stop();

    System::cleanTemporaries();

    return 0;
}
