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

namespace Gui
{
    namespace
    {

        void gui_thread(int argc, char **argv)
        {
            QApplication app(argc, argv);

            app.exec();
        }

        struct GuiThread
        {
            void init(int argc, char **argv)
            {
                assert(m_thread == nullptr);
            }

            void run()
            {
                m_thread = new std::thread(gui_thread, argc, argv);
            }

            void quit()
            {
                assert(m_thread != nullptr);

                m_thread.join();
            }

            static GuiThread* get()
            {
            }

            private:
                GuiThread(int argc, char **argv): m_thread(nullptr), m_argc(0), m_argv(nullptr)
                {

                }

                std::thread *m_thread;
                int m_argc;
                char **m_argv;
        };
    }


    Initializator::~Initializator()
    {

    }


    int Initializator::run()
    {
        GuiThread::get()->run();
        GuiThread::get()->quit();
		return 0;
    }

    void Initializator::init(int argc, char** argv)
    {
        GuiThread::get()->init(argc, argv);
    }

}
