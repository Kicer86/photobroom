
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
#include <system/system.hpp>

int main(int argc, char **argv)
{
    // build objects
    DefaultConfiguration configuration;

    Logger logger;

    PluginLoader pluginLoader;
    Database::Builder database_builder;
    database_builder.set(&pluginLoader);
    database_builder.set(&configuration);
    database_builder.set(&logger);

    ProjectManager prjManager;
    prjManager.set(&database_builder);

    // init configuration
    configuration.load();

    const std::string basePath = System::getApplicationConfigDir() + "/logs";
    auto severity = Logger::Severity::Debug;
    logger.setPath(basePath);
    logger.setLevel(severity);

    // start gui
    Gui gui;
    gui.set(&prjManager);
    gui.set(&pluginLoader);
    gui.run(argc, argv);

    return 0;
}
