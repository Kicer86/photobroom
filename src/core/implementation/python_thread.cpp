/*
 * Utils responsible for managing Python's thread.
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


#include <system/filesystem.hpp>

#include "python_utils.hpp"


namespace
{
    void init_python()
    {
        Py_Initialize();

        int status = 0;

        // add path to scripts to Python's search path
        // https://mail.python.org/pipermail/capi-sig/2013-May/000590.html

        const QString scripts_qstr = FileSystem().getScriptsPath();
        const std::string scripts_str = scripts_qstr.toStdString();

        PyObjPtr scriptsPath( PyUnicode_FromString(scripts_str.c_str()) );
        PyObjPtr pPathsList( PySys_GetObject("path") );
        status = PyList_Append(pPathsList.get(), scriptsPath.get());
        assert(status == 0);

        status = PySys_SetObject("path", pPathsList.get());
        assert(status == 0);
    }

    void deinit_python()
    {
        Py_Finalize();
    }
}


PythonThread::PythonThread(): m_tasks(16), m_pythonThread(std::bind(&PythonThread::thread, this))
{
    execute(&init_python);
}


PythonThread::~PythonThread()
{
    execute(&deinit_python);
    m_tasks.stop();
    m_pythonThread.join();
}


void PythonThread::execute(const std::function<void ()>& callback)
{
    m_tasks.push(callback);
}


void PythonThread::thread()
{
    for(;;)
    {
        auto task = m_tasks.pop();

        if (task)
        {
            const auto& callable = *task;
            callable();
        }
        else
            break;
    }
}

