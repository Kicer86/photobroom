/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "python_thread.hpp"

#include <iostream>

#include <pybind11/embed.h>

#include <core/thread_utils.hpp>
#include <system/filesystem.hpp>


namespace
{
    void init_python()
    {
        const QString scripts_qstr = FileSystem().getScriptsPath();
        const std::string scripts_str = scripts_qstr.toStdString();

        const QString libs_qstr = FileSystem().getLibrariesPath();
        const std::string libs_str = libs_qstr.toStdString();
        const std::string python_modules_str = libs_str + "/python_modules";

        try
        {
            pybind11::module sys = pybind11::module::import("sys");
            sys.attr("path").cast<pybind11::list>().append(scripts_str);
            sys.attr("path").cast<pybind11::list>().append(python_modules_str);
            pybind11::print(sys.attr("path"));
        }
        catch (const std::exception& err)
        {
            std::cout << err.what() << std::endl;
        }
    }

    void deinit_python()
    {
        // Nothing special to do
    }
}


struct PythonThread::Impl
{
    pybind11::scoped_interpreter m_py_interpreter;
    ol::TS_Queue<std::unique_ptr<ITask>> m_tasks;

    Impl(): m_py_interpreter(), m_tasks(16)
    {

    }
};


PythonThread::PythonThread(): m_impl(nullptr), m_pythonThread()
{
#ifdef OS_WIN
    Py_NoSiteFlag = 1;
#endif
    m_impl = std::make_unique<Impl>();
    m_pythonThread = std::thread(std::bind(&PythonThread::thread, this));
    execute(&init_python);
}


PythonThread::~PythonThread()
{
    execute(&deinit_python);
    m_impl->m_tasks.stop();
    m_pythonThread.join();
}


void PythonThread::execute(std::unique_ptr<ITask>&& task)
{
    m_impl->m_tasks.push(std::move(task));
}


void PythonThread::thread()
{
    set_thread_name("Python");

    for(;;)
    {
        auto task = m_impl->m_tasks.pop();

        if (task)
            (*task)->run();
        else
            break;
    }
}

