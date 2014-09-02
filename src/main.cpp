
#include <assert.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include <core/plugin_loader.hpp>
#include <configuration/configurationfactory.hpp>
#include <configuration/iconfiguration.hpp>
#include <database/database_builder.hpp>
#include <gui/gui.hpp>
#include <project_utils/project_manager.hpp>

int main(int argc, char **argv)
{
    //init modules
    Database::Builder database_builder;
    database_builder.initConfig();

    //load configuration
    ConfigurationFactory::get()->load();

    //build objects
    PluginLoader pluginLoader;
    //Database::Builder database_builder;
    database_builder.set(&pluginLoader);

    ProjectManager prjManager;
    prjManager.set(&database_builder);

    //start gui
    Gui gui;
    gui.set(&prjManager);
    gui.run(argc, argv);

    return 0;
}
