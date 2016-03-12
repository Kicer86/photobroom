
#include <assert.h>

#include <vector>
#include <string>

#include <QCoreApplication>

#include <core/logger_factory.hpp>
#include <core/task_executor.hpp>
#include <core/ilogger.hpp>
#include <configuration/configuration.hpp>
#include <database/database_builder.hpp>
#include <gui/gui.hpp>
#include <plugins/plugin_loader.hpp>
#include <project_utils/project_manager.hpp>
#include <system/system.hpp>
#include <system/crash_catcher.hpp>


int main(int argc, char **argv)
{
    Gui gui;

    std::unique_ptr<QCoreApplication> app = gui.init(argc, argv);
    app->setApplicationName("photo_broom");                                // without this app name may change when binary name changes

    const bool status = CrashCatcher::init(argv[0]);
    const QString basePath = System::getApplicationConfigDir() + "/logs";

    // build objects
    LoggerFactory logger_factory(basePath);

    Configuration configuration;

    PluginLoader pluginLoader;
    pluginLoader.set(&logger_factory);

    TaskExecutor taskExecutor;

    Database::Builder database_builder;
    database_builder.set(&pluginLoader);
    database_builder.set(&logger_factory);
    database_builder.set(&configuration);

    ProjectManager prjManager;
    prjManager.set(&database_builder);

    if (status)
        logger_factory.get("CrashCatcher")->debug("Initialization successful");
    else
        logger_factory.get("CrashCatcher")->error("Initialization failed");

    // start gui
    gui.set(&prjManager);
    gui.set(&pluginLoader);
    gui.set(&configuration);
    gui.set(&logger_factory);
    gui.set(&taskExecutor);
    gui.run();

    return 0;
}
