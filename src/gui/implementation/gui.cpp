
#include "gui.hpp"

#include "implementation/Qt/initializator.hpp"

namespace Gui
{
    std::shared_ptr<IUi> Factory::get()
    {
        static std::shared_ptr<IUi> result;

        if (result.get() == nullptr)
            result.reset(new Initializator);

        return result;
    }
}
