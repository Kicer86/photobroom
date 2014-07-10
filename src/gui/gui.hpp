
/* GUI interface */

#ifndef GUI_HPP
#define GUI_HPP

#include <memory>

#include "gui_export.h"

namespace Gui
{

    struct IUi
    {
        virtual ~IUi() {}
        virtual void run (int argc, char **argv) = 0;
    };


    struct GUI_EXPORT Factory
    {
        static std::unique_ptr<IUi> get();
    };
}

#endif
