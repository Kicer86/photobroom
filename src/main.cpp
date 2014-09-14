
#include <assert.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include <core/plugin_loader.hpp>
#include <core/logger.hpp>
#include <configuration/configuration.hpp>
#include <database/database_builder.hpp>
#include <gui/gui.hpp>
#include <project_utils/project_manager.hpp>

int main(int argc, char **argv)
{
    // build objects
    Logger logger;
    DefaultConfiguration configuration;

    PluginLoader pluginLoader;
    Database::Builder database_builder;
    database_builder.set(&pluginLoader);
    database_builder.set(&configuration);

    ProjectManager prjManager;
    prjManager.set(&database_builder);

    // init configuration
    configuration.load();

    // start gui
    Gui gui;
    gui.set(&prjManager);
    gui.set(&pluginLoader);
    gui.run(argc, argv);

    return 0;
}
