
#include <assert.h>

#include <vector>
#include <string>

#include <core/logger_factory.hpp>
#include <core/task_executor.hpp>
#include <configuration/configuration.hpp>
#include <database/database_builder.hpp>
#include <gui/gui.hpp>
#include <plugins/plugin_loader.hpp>
#include <project_utils/project_manager.hpp>
#include <system/system.hpp>


int main(int argc, char **argv)
{
    const QString basePath = System::getApplicationConfigDir() + "/logs";

    // build objects
    LoggerFactory logger_factory(basePath);

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
