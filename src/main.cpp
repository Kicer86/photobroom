
#include <assert.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "gui/gui.hpp"
#include "configuration/configurationfactory.hpp"
#include "configuration/iconfiguration.hpp"
#include "database/database_builder.hpp"

int main(int argc, char **argv)
{
    //init modules
    Database::Builder::instance()->initConfig();

    //load configuration
    ConfigurationFactory::get()->load();

    //start gui
    Gui gui;
    gui.run(argc, argv);

    return 0;
}
