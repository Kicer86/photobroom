
/* GUI itnerface */

#ifndef GUI_HPP
#define GUI_HPP

#include <memory>

namespace Gui
{
 
    struct IUi
    {
        virtual ~IUi() {}
        virtual void init (int argc, char **argv) = 0;
        virtual int run() = 0;
        virtual void quit() = 0;
    };
    
    
    struct Factory
    {
        static std::shared_ptr<IUi> get();
    };
}

#endif
