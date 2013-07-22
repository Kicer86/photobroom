
#include <assert.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "gui/gui.hpp"

int main(int argc, char **argv)
{
    std::shared_ptr<Gui::IUi> gui = Gui::Factory::get();

    gui->run(argc, argv);

    return 0;
}
