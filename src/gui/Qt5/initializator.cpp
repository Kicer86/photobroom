/*
    Qt based gui initializator
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "initializator.hpp"

#include <assert.h>

#include <thread>

#include <QApplication>

#include "mainwindow.hpp"

namespace Gui
{
    namespace
    {

        struct GuiThread
        {
            void run(int argc, char **argv)
            {
                QApplication app(argc, argv);
                MainWindow mainWindow;

                mainWindow.show();
                app.exec();
            }

            static GuiThread* get()
            {
                static GuiThread instance;
                
                return &instance;
            }

            private:
                GuiThread()
                {

                }
        };
    }


    Initializator::~Initializator()
    {

    }


    void Initializator::run(int argc, char** argv)
    {
        GuiThread::get()->run(argc, argv);
    }

}
