
/* GUI interface */

#ifndef GUI_HPP
#define GUI_HPP

#include "gui_export.h"

struct IPluginLoader;
struct IProjectManager;

struct GUI_EXPORT Gui
{
    Gui();
    Gui(const Gui &) = delete;
    Gui& operator=(const Gui &) = delete;

    void set(IProjectManager *);
    void set(IPluginLoader *);
    void run(int argc, char **argv);

    private:
        IProjectManager* m_prjManager;
        IPluginLoader* m_pluginLoader;
};

#endif
