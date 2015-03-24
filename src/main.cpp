
#include <assert.h>

#include <vector>
#include <string>

#include <core/plugin_loader.hpp>
#include <core/logger_factory.hpp>
#include <core/task_executor.hpp>
#include <configuration/configuration.hpp>
#include <database/database_builder.hpp>
#include <gui/gui.hpp>
#include <project_utils/project_manager.hpp>
#include <system/system.hpp>

int main(int argc, char **argv)
{
    // build objects
    LoggerFactory logger_factory;

    DefaultConfiguration configuration;
    configuration.init(&logger_factory);

    PluginLoader pluginLoader;
    pluginLoader.set(&logger_factory);

    TaskExecutor taskExecutor;
    
    Database::Builder database_builder;
    database_builder.set(&pluginLoader);
    database_builder.set(&logger_factory);
    database_builder.set(&configuration);

    ProjectManager prjManager;
    prjManager.set(&database_builder);
    prjManager.set(&configuration);

    // init configuration
    configuration.load();

    const QString basePath = System::getApplicationConfigDir() + "/logs";
    logger_factory.setPath(basePath);

    // start gui
    Gui gui;
    gui.set(&prjManager);
    gui.set(&pluginLoader);
    gui.set(&configuration);
    gui.set(&logger_factory);
    gui.set(&taskExecutor);
    gui.run(argc, argv);

    return 0;
}
