
#include <assert.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "gui/gui.hpp"
#include "configuration/configurationfactory.hpp"
#include "configuration/iconfiguration.hpp"

int main(int argc, char **argv)
{
    //load configuration
    ConfigurationFactory::get()->load();

    //start gui
    std::shared_ptr<Gui::IUi> gui = Gui::Factory::get();
    gui->run(argc, argv);

    return 0;
}
