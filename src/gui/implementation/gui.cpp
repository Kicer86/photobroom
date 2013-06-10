
#include "gui.hpp"

#include "implementation/Qt/initializator.hpp"

namespace Gui
{
    std::shared_ptr<IUi> Factory::get()
    {
		IUi *ui = new Initializator;

		return std::shared_ptr<IUi>(ui);
    }
}
