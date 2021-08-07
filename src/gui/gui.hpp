
/* GUI interface */

#ifndef GUI_HPP
#define GUI_HPP

#include <memory>

#include "gui_export.h"

class QCoreApplication;

struct ICoreFactoryAccessor;
struct IPluginLoader;
struct IProjectManager;
class IFeaturesManager;

struct GUI_EXPORT Gui
{
    Gui(IProjectManager &, IPluginLoader &, ICoreFactoryAccessor &, IFeaturesManager &);
    ~Gui();
    Gui(const Gui &) = delete;
    Gui& operator=(const Gui &) = delete;

    void run();

    private:
        IProjectManager& m_prjManager;
        IPluginLoader& m_pluginLoader;
        ICoreFactoryAccessor& m_coreFactory;
        IFeaturesManager& m_featuresManager;
};

#endif
