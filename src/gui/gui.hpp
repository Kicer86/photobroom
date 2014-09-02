
/* GUI interface */

#ifndef GUI_HPP
#define GUI_HPP

#include "gui_export.h"

class IProjectManager;

struct GUI_EXPORT Gui
{
    Gui();
    Gui(const Gui &) = delete;
    Gui& operator=(const Gui &) = delete;

    void set(IProjectManager *);
    void run(int argc, char **argv);

    private:
        IProjectManager* m_prjManager;
};

#endif
