
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include <QApplication>
#include <QDir>
#include <QImageReader>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTimer>

#ifdef OS_WIN
#include <Windows.h>
#endif

#include <core/core_factory_accessor.hpp>
#include <core/exif_reader_factory.hpp>
#include <core/logger_factory.hpp>
#include <core/task_executor.hpp>
#include <core/ifeatures_manager.hpp>
#include <core/ilogger.hpp>
#include <crash_catcher/crash_catcher.hpp>
#include <database/database_builder.hpp>
#include <gui/gui.hpp>
#include <plugins/plugin_loader.hpp>
#include <project_utils/project_manager.hpp>
#include <system/system.hpp>

#include "paths.hpp"
#include "config_storage.hpp"
#include "features.hpp"


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
    QApplication app(argc, argv);
    app.setApplicationName("photo_broom");                   // without this app name may change when binary name changes

    const QString basePath = System::getApplicationConfigDir();

    // setup internal locations
    QDir::setSearchPaths("icons", {QString(Paths::icons) + "/svg"});

    // perform command line parsing
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption logingLevelOption("loging-level",
                                         QCoreApplication::translate("main", "Defines loging level. Possible options are: Trace, Debug, Info, Warning (default), Error"),
                                         QCoreApplication::translate("main", "loging level"),
                                         "Warning"
    );

    QCommandLineOption developerOptions("feature-toggle",
                                         QCoreApplication::translate("main", "Enables experimental features. Use for each flag you want to turn on: test-crash-catcher, debug"),
                                         QCoreApplication::translate("main", "flag")
    );

    QCommandLineOption disableCrashCatcher("disable-crash-catcher", "Turns off crash catcher");

    QCommandLineOption imageMemoryLimit("image-memory-limit",
                                        QCoreApplication::translate("main", "Limit for image sizes in megabytes. 512MB by default, which allows to open 512/4 = 128MPix photos."),
                                        QCoreApplication::translate("main", "image memory limit"),
                                        "512"
    );

#ifdef OS_WIN
    QCommandLineOption enableConsole("enable-console", "Opens console with app's output messages");
    enableConsole.setFlags(QCommandLineOption::HiddenFromHelp);

    parser.addOption(enableConsole);
#endif

    parser.addOption(logingLevelOption);
    parser.addOption(developerOptions);
    parser.addOption(disableCrashCatcher);
    parser.addOption(imageMemoryLimit);

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

    const QString logingLevelStr = parser.value(logingLevelOption);
    ILogger::Severity logingLevel = ILogger::Severity::Warning;

    if (logingLevelStr == "Trace")
        logingLevel = ILogger::Severity::Trace;
    else if (logingLevelStr == "Debug")
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
    if (featureToggles.contains("debug"))
        toggles.insert(cmdline_fatures::Debug);
    CommandLineToggles cmdLineToggles(toggles);

    LoggerFactory logger_factory(basePath);
    logger_factory.setLogingLevel(logingLevel);

    const QString configFileDir = System::getApplicationConfigDir();
    const QString configFilePath = configFileDir + "/" + "config.json";
    ConfigStorage configStorage(configFilePath);
    Configuration configuration(configStorage);

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
