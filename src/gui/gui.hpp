
/* GUI interface */

#ifndef GUI_HPP
#define GUI_HPP

#include "gui_export.h"

struct GUI_EXPORT Gui
{
    Gui();
    Gui(const Gui &) = delete;
    Gui& operator=(const Gui &) = delete;

    void run(int argc, char **argv);
};

#endif
