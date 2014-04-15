
#include "gui.hpp"

#include <QApplication>

#include "mainwindow.hpp"

namespace Gui
{
    namespace
    {
        struct Gui: IUi
        {
            Gui() {}
            Gui(const Gui &) = delete;
            Gui& operator=(const Gui &) = delete;

            void run(int argc, char **argv)
            {
                QApplication app(argc, argv);
                MainWindow mainWindow;

                mainWindow.show();
                app.exec();
            }
        };
    }


    std::unique_ptr<IUi> Factory::get()
    {
        return std::unique_ptr<IUi>(new Gui);
    }
}
