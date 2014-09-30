
#include <assert.h>

#include <vector>
#include <string>

#include <core/plugin_loader.hpp>
#include <core/logger.hpp>
#include <core/task_executor.hpp>
#include <configuration/configuration.hpp>
#include <database/database_builder.hpp>
#include <gui/gui.hpp>
#include <project_utils/project_manager.hpp>
#include <system/system.hpp>

int main(int argc, char **argv)
{
    // build objects
    Logger logger;

    DefaultConfiguration configuration;
    configuration.init(&logger);

    PluginLoader pluginLoader;
    pluginLoader.set(&logger);

    TaskExecutor taskExecutor;
    
    Database::Builder database_builder;
    database_builder.set(&pluginLoader);
    database_builder.set(&configuration);
    database_builder.set(&logger);
    database_builder.set(&taskExecutor);

    ProjectManager prjManager;
    prjManager.set(&database_builder);

    // init configuration
    configuration.load();

    const QString basePath = System::getApplicationConfigDir() + "/logs";
    auto severity = Logger::Severity::Debug;
    logger.setPath(basePath);
    logger.setLevel(severity);

    // start gui
    Gui gui;
    gui.set(&prjManager);
    gui.set(&pluginLoader);
    gui.set(&configuration);
    gui.run(argc, argv);

    return 0;
}
