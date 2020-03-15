
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
    Gui();
    ~Gui();
    Gui(const Gui &) = delete;
    Gui& operator=(const Gui &) = delete;

    void set(IProjectManager *);
    void set(IPluginLoader *);
    void set(ICoreFactoryAccessor *);
    void run();

    private:
        IProjectManager* m_prjManager;
        IPluginLoader* m_pluginLoader;
        ICoreFactoryAccessor* m_coreFactory;
};

#endif

