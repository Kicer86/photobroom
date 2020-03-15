
/* GUI interface */

#ifndef GUI_HPP
#define GUI_HPP

#include <memory>

#include "gui_export.h"

class QCoreApplication;

struct ICoreFactoryAccessor;
struct IPluginLoader;
struct IProjectManager;

struct GUI_EXPORT Gui
{
    Gui(IProjectManager &, IPluginLoader &, ICoreFactoryAccessor &);
    ~Gui();
    Gui(const Gui &) = delete;
    Gui& operator=(const Gui &) = delete;

    void run();

    private:
        IProjectManager& m_prjManager;
        IPluginLoader& m_pluginLoader;
        ICoreFactoryAccessor& m_coreFactory;
};

#endif

