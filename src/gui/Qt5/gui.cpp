
#include "gui.hpp"

#include <QApplication>

#include "mainwindow.hpp"


Gui::Gui()
{

}


void Gui::run(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow mainWindow;

    mainWindow.show();
    app.exec();
}
