
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
    Gui(int& argc, char **argv);
    ~Gui();
    Gui(const Gui &) = delete;
    Gui& operator=(const Gui &) = delete;

    QCoreApplication* getApp();
    void set(IProjectManager *);
    void set(IPluginLoader *);
    void set( ICoreFactoryAccessor *);
    void run();

    private:
        std::unique_ptr<QCoreApplication> m_app;
        IProjectManager* m_prjManager;
        IPluginLoader* m_pluginLoader;
        ICoreFactoryAccessor* m_coreFactory;
};

#endif

